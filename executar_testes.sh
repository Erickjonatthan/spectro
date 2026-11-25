#!/usr/bin/env bash
# bash executar_testes.sh -- Script para compilar e executar todos os testes da linguagem Spectro.xum
set -euo pipefail

cd "$(dirname "$0")"

echo "Compilando o interpretador Spectro.xum..."
make clean
make

temp_dir=$(mktemp -d)
trap 'rm -rf -- "$temp_dir"' EXIT

pass_count=0
fail_count=0

extract_expectations() {
    local test_file=$1
    local line

    expected_exit=""
    : > "$temp_dir/expected.stdout"
    : > "$temp_dir/expected.stderr"
    : > "$temp_dir/contains.stdout"
    : > "$temp_dir/contains.stderr"

    while IFS= read -r line || [[ -n "$line" ]]; do
        line=${line%$'\r'}
        case "$line" in
            "? EXPECT-EXIT: "*)
                expected_exit=${line#"? EXPECT-EXIT: "}
                ;;
            "? EXPECT-STDOUT: "*)
                printf '%s\n' "${line#"? EXPECT-STDOUT: "}" >> "$temp_dir/expected.stdout"
                ;;
            "? EXPECT-STDERR: "*)
                printf '%s\n' "${line#"? EXPECT-STDERR: "}" >> "$temp_dir/expected.stderr"
                ;;
            "? EXPECT-STDOUT-CONTAINS: "*)
                printf '%s\n' "${line#"? EXPECT-STDOUT-CONTAINS: "}" >> "$temp_dir/contains.stdout"
                ;;
            "? EXPECT-STDERR-CONTAINS: "*)
                printf '%s\n' "${line#"? EXPECT-STDERR-CONTAINS: "}" >> "$temp_dir/contains.stderr"
                ;;
        esac
    done < "$test_file"

    if [[ ! "$expected_exit" =~ ^[0-9]+$ ]]; then
        echo "  FALHA: expectativa EXPECT-EXIT ausente ou inválida em $test_file"
        return 1
    fi

    return 0
}

compare_stream() {
    local actual_file=$1
    local expected_file=$2
    local contains_file=$3
    local stream_name=$4
    local expected_line

    if [[ -s "$contains_file" ]]; then
        while IFS= read -r expected_line || [[ -n "$expected_line" ]]; do
            if ! grep -Fq -- "$expected_line" "$actual_file"; then
                echo "  FALHA: $stream_name não contém o trecho esperado: $expected_line"
                echo "  Saída obtida:"
                cat "$actual_file"
                return 1
            fi
        done < "$contains_file"
    elif ! cmp -s "$expected_file" "$actual_file"; then
        echo "  FALHA: $stream_name diferente do esperado"
        diff -u "$expected_file" "$actual_file" || true
        return 1
    fi

    return 0
}

run_case() {
    local test_file=$1
    local actual_exit
    local failed=0

    echo "--- $test_file"
    if ! extract_expectations "$test_file"; then
        fail_count=$((fail_count + 1))
        return
    fi

    if ./mini "$test_file" > "$temp_dir/actual.stdout" 2> "$temp_dir/actual.stderr"; then
        actual_exit=0
    else
        actual_exit=$?
    fi

    if [[ "$actual_exit" != "$expected_exit" ]]; then
        echo "  FALHA: código de saída esperado $expected_exit, obtido $actual_exit"
        failed=1
    fi

    if ! compare_stream "$temp_dir/actual.stdout" "$temp_dir/expected.stdout" "$temp_dir/contains.stdout" "stdout"; then
        failed=1
    fi
    if ! compare_stream "$temp_dir/actual.stderr" "$temp_dir/expected.stderr" "$temp_dir/contains.stderr" "stderr"; then
        failed=1
    fi

    if [[ "$failed" -eq 0 ]]; then
        echo "  PASSOU"
        pass_count=$((pass_count + 1))
    else
        fail_count=$((fail_count + 1))
    fi
}

for test_file in programa1.xum programa2.xum programa3.xum programa4.xum; do
    run_case "$test_file"
done

for test_file in testes/validos/*.xum; do
    run_case "$test_file"
done

for test_file in testes/invalidos/*.xum; do
    run_case "$test_file"
done

echo ""
echo "Resultado: $pass_count aprovados; $fail_count com falha."

if [[ "$fail_count" -gt 0 ]]; then
    exit 1
fi
