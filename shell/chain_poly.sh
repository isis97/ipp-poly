#!/bin/bash
#
# @Piotr Styczyński 2017
#

FDIRECTIVE_START=START

temp_location_input=./chain.temp
temp_location_output=./chain.out.temp
arg_prog=
arg_test_dir=
flag_formating=none
flag_verbose=false
flag_silent_errors=false

number_of_tests=0
start_file=
flag_directive_file=

file_history=

C_RED=$(printf "\e[1;31m")
C_GREEN=$(printf "\e[1;32m")
C_BLUE=$(printf "\e[1;34m")
C_CYAN=$(printf "\e[1;36m")
C_PURPLE=$(printf "\e[1;35m")
C_YELLOW=$(printf "\e[1;33m")
C_GRAY=$(printf "\e[1;30m")
C_NORMAL=$(printf "\e[0m")
C_BOLD=$(printf "\e[1m")

B_DEBUG=
E_DEBUG=
B_ERR=
E_ERR=
B_INFO=
E_INFO=
B_WARN=
E_WARN=
B_BOLD=
E_BOLD=
B_OK=
E_OK=

function print_help {
  printf "\n"
  printf "Usage:\n"
  printf "    chain_poly  [test_flags] <prog> <dir>\n\n"
  printf "      <prog> is path to the poly calculator executable\n"
  printf "      <dir> is path to the folder containing tests\n"
  printf "            Tests are poly caluclator valid input files\n"
  printf "            containing additionaly special markers:\n"
  printf "               START - appears in the first line of files\n"
  printf "                       indicates that is the first file to be used\n"
  printf "               STOP  - parser stops after current file\n"
  printf "               FILE <filename> - parser load current file and joins\n"
  printf "                       its output with the file <filename> then parses\n"
  printf "                       the concatenated file.\n"
  printf "      Flags are:\n"
  printf "        -s  - silent mode\n"
  printf "            do not display errors, display last valid output of calculator\n"
  printf "        -v  - verbose mode\n"
  printf "            display additional debug info (like file routing etc.)\n"
  printf "        -none-format - use no formatting with produced output\n"
  printf "        -term-format - use default term formatting with produced output\n"
  printf "        -sty-format - use special sty formatting with produced output\n"
  printf "\n"
  exit 0
}

function safe_exit {
  #if [ "$flag_silent_errors" = "true" ]; then
  #  print_results
  #fi
  clean_temp_files
  exit $1
}

function set_format {
  if [[ ${flag_formating} = 'sty' ]]; then
    B_DEBUG="!debug!"
    E_DEBUG="!normal!"
    B_ERR="!error!"
    E_ERR="!normal!"
    B_INFO="!info!"
    E_INFO="!normal!"
    B_WARN="!warn!"
    E_WARN="!normal!"
    B_BOLD="!bold!"
    E_BOLD="!normal!"
    B_OK="!ok!"
    E_OK="!normal!"
  fi
  if [[ ${flag_formating} = 'term' ]]; then
    B_DEBUG=$C_GRAY
    E_DEBUG=$C_NORMAL
    B_ERR=$C_RED
    E_ERR=$C_NORMAL
    B_INFO=$C_BLUE
    E_INFO=$C_NORMAL
    B_WARN=$C_YELLOW
    E_WARN=$C_NORMAL
    B_BOLD=$C_BOLD
    E_BOLD=$C_NORMAL
    B_OK=$C_GREEN
    E_OK=$C_NORMAL
  fi
}

function scan_directory {
  number_of_tests=0
  start_file=
  if [ -d "$1" ]; then
    for test_file in "$1"/** "$1"/**/* ; do
      if [ -f "$test_file" ]; then
        test_head=$(head -n 1 "$test_file")
        if [ "$test_head" = "$FDIRECTIVE_START" ]; then
          start_file="$test_file"
        fi
        number_of_tests=$((number_of_tests+1))
      fi
    done
    if [ "$start_file" = "" ]; then
      if [ "$flag_silent_errors" = "false" ]; then
        printf "${B_ERR}Start file not found in $1.\nPlease add a file with head directive: $FDIRECTIVE_START${E_ERR}\n"
      fi
      safe_exit 1
    fi
    if [ "$flag_verbose" = "true" ]; then
      printf "${B_OK}Found $number_of_tests test/-s in the given location and HEAD: ${start_file}.${E_OK}\n"
    fi
  fi
}

function run_chained_test {

  file_history_match=$(printf "$file_history\n" | grep -B0 -A999999999 -F -x "$1")
  if [ "$file_history_match" != "" ]; then
    file_history_trace=$(echo "$file_history_match" | sed -r 's/^(.*)$/  |  File \1/g')
    if [ "$flag_silent_errors" = "false" ]; then
      printf "${B_ERR}Circular dependecy in file: $1\nTrace:\n$file_history_trace${E_ERR}\n"
    fi
    flag_directive_file=""
    safe_exit 1
  fi

  file_history="$file_history\n$1"
  if [ "$flag_verbose" = "true" ]; then
    printf "${B_DEBUG}Parse file: $1${E_DEBUG}\n"
  fi
  cat "$1" | sed -r '/^((FILE|START|STOP)).*$/d' >> "$temp_location_input"

  "$arg_prog" < "$temp_location_input" > "$temp_location_output"
  cat "$temp_location_output" > "$temp_location_input"
  rm "$temp_location_output"

  flag_directive_file=

  input=$(cat "$1" | sed -r '/^((FILE|STOP)).*$/!d')

  while read -r line; do
    if echo "$line" | grep -q "FILE "; then
      file_name=$(echo "$line" | sed -r 's/FILE //g')
      file_name="$arg_test_dir/$file_name"
      if [ "$flag_verbose" = "true" ]; then
        printf "${B_INFO}Redirect to file $file_name${E_INFO}\n"
      fi
      if [ "$flag_directive_file" = "" ]; then
        flag_directive_file="$file_name"
      else
        if [ "$flag_silent_errors" = "false" ]; then
          printf "${B_ERR}Multiple FILE directives in file: $1${E_ERR}\n"
        fi
        flag_directive_file=""
        safe_exit 1
      fi
    elif [ "$line" = "STOP" ]; then
      if [ "$flag_verbose" = "true" ]; then
        printf "${B_DEBUG}Stop directive met${E_DEBUG}\n"
      fi
      flag_directive_file=""
      break
    fi
  done <<< "$input"
}

function run_chained_tests {
  if [ -f "$temp_location_input" ]; then
    rm "$temp_location_input"
  fi
  echo "" > "$temp_location_input"
  flag_directive_file="$start_file"
  while [ "$flag_directive_file" != "" ]; do
    run_chained_test "$flag_directive_file"
  done
}

function print_results {
  cat "$temp_location_input"
}

function clean_temp_files {
  if [ -f "$temp_location_input" ]; then
    rm "$temp_location_input"
  fi
}

function test_params {
  if [ ! -f "$arg_prog" ]; then
    if [ "$flag_silent_errors" = "false" ]; then
      printf "${B_ERR}Given program doest not exist: ${arg_prog}${E_ERR}\n"
    fi
    safe_exit 1
  fi

  if [ ! -d "$arg_test_dir" ]; then
    if [ "$flag_silent_errors" = "false" ]; then
      printf "${B_ERR}Given test directory does not exist: ${arg_test_dir}${E_ERR}\n"
    fi
    safe_exit 1
  fi
}



set_format

nonflag_param_count=0
while test $# != 0
do
  case "$1" in
    -help) print_help ;;
    --help) print_help ;;
    -v) flag_verbose=true ;;
    -s) flag_silent_errors=true ;;
    -sty-format) flag_formating=sty ;;
    -term-format) flag_formating=term ;;
    -none-format) flag_formating=none ;;
    -*) {
      if [ "$flag_silent_errors" = "false" ]; then
        printf "${B_ERR}Unknown switch was used: $1${E_ERR}"
      fi
      safe_exit 1
    } ;;
    *) {
      if [ "$nonflag_param_count" = "0" ]; then
        arg_prog="$1"
      elif [ "$nonflag_param_count" = "1" ]; then
        arg_test_dir="$1"
      else
        if [ "$flag_silent_errors" = "false" ]; then
          printf "${B_ERR}Wrong number of parameters (too much paths was given?)${E_ERR}\n"
        fi
        safe_exit 1
      fi
      nonflag_param_count=$((nonflag_param_count+1))
    } ;;
  esac
  shift
done


set_format
test_params
scan_directory "$arg_test_dir"
run_chained_tests

if [ "$flag_verbose" = "true" ]; then
  printf "${B_OK}Done without errors.${E_OK}\n\n"
fi

print_results
clean_temp_files
exit 0
