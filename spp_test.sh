#Original author: Kartal Kaan Bozdoğan (mstniy)
#Source repository: https://github.com/mstniy/safepm

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

function print_line {
  linelen=${#1}
  echo -n "$1"
}

function erase_line {
  echo -ne "\r"
  for ((i=0; i<$linelen; i++))
  do
    echo -n " "
  done
  echo -ne "\r"
}

function should_crash {
  snippet="$1"
  command="$2"
  shift 2
  
  print_line "Running $command $@..."

  output=`{ "$command" "$@" 2>&1; }` && { erase_line ; echo -e "${RED}$command did not crash.${NC}"; return 1; }
  erase_line
  if ! ( echo "$output" | grep PMDK_ASAN_PASS_FLAG) >/dev/null # Crashed too early
  then
      echo -e "${RED}Test for $command failed.${NC}"
      return 1
  fi
  if ( echo "$output" | grep PMDK_ASAN_FAIL_FLAG) >/dev/null # Crashed too late
  then
      echo -e "${RED}Test for $command failed.${NC}"
      return 1
  fi
  (echo "$output" | grep -E -- "$snippet") >/dev/null && echo -e "${GREEN}$command OK.${NC}" || echo -e "${RED}Test for $command failed.${NC}"
}

function should_not_crash {
  command="$1"
  shift 1
  
  print_line "Running $command $@..."
  
  ( "$command" "$@" >/dev/null 2>&1 ) || { erase_line; echo -e "${RED}$command crashed.${NC}"; return 1; }
  erase_line
  echo -e "${GREEN}$command OK.${NC}"
}

function pending_should_crash {
  echo -e "${BLUE}$2 pending.${NC}"
}

SPPLIBOBJ='./runtime/obj'

WRAP_LIST='-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul'   

CFLAGS='-flto -O2 -Xclang -load -Xclang ./llvm-project/build/lib/LLVMSPP.so -include ./runtime/src/spp.h'

CXXFLAGS='-flto -O2 -Xclang -load -Xclang ./llvm-project/build/lib/LLVMSPP.so -include ./runtime/src/spp.h'

LDFLAGS='-fuse-ld=gold -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul ./runtime/obj/wrappers.o -Xlinker ./runtime/obj/spp_hookobj.o'

echo "1__CXXFLAGS: ${CXXFLAGS}"

cd "$(dirname "$0")"
mkdir -p build
cd build
[ -e Makefile ] && make clean

echo "__CLANG:    $(which clang)"
echo "__CLANG++:  $(which clang++)"
echo "__GOLD:     $(which ld.gold)"
echo "__CXXFLAGS: ${CXXFLAGS}"
echo "__LDFLAGS:  ${LDFLAGS}"

NDEBUG=1;

if [ "$NDEBUG" = "1" ]
then
  cmake ..  -DCMAKE_C_COMPILER=clang  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="${CXXFLAGS}" -DCMAKE_CFLAGS="${CFLAGS}" -DCMAKE_LDFLAGS="${LDFLAGS}" -DCMAKE_BUILD_TYPE=RelWithDebInfo 
    #cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
    echo "___N_Debug_________________"
else
  cmake ..  -DCMAKE_C_COMPILER=clang  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="${CXXFLAGS}" -DCMAKE_CFLAGS="${CFLAGS}" -DCMAKE_LDFLAGS="${LDFLAGS}" -DCMAKE_BUILD_TYPE=RelWithDebInfo 
    #cmake ..  -DCMAKE_BUILD_TYPE=Debug
    echo "___Debug___________________"
fi

make -j7

cd tests

set +e
#should_crash "Invalid free" ./mismatched_free.exe
#should_crash "(Invalid free| palloc_heap_action_exec\] assertion failure)" ./double_free.exe
#should_crash "(Invalid free| palloc_heap_action_exec\] assertion failure)" ./double_free_ntx.exe
#should_crash "\[fd\]" ./use_after_free.exe
#should_crash "\[fd\]" ./use_after_free_ntx.exe
#should_crash "\[fd\]" ./use_after_realloc.exe
#should_crash "00\[fa\]" ./overflow.exe
#should_crash "00\[fa\]" ./overflow_ntx.exe
#should_crash "00\[fa\]" ./root_overflow.exe
#should_crash "00\[fa\]" ../../tests/overflow_persistence.sh
#should_crash "\[fa\]" ./root_underflow.exe
#should_crash "\[04\]" ./int32.exe
#should_crash "\[fd\]" ./alloc_tx_abort.exe
#should_not_crash ./free_tx_abort.exe
#should_crash "00\[fa\]" ./tx_add_overflow.exe
#should_not_crash ./zalloc.exe
