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
      echo -e "${RED}Test for $command failed. Crashed too early.${NC}"
      return 1
  fi
  if ( echo "$output" | grep PMDK_ASAN_FAIL_FLAG) >/dev/null # Crashed too late
  then
      echo -e "${RED}Test for $command failed. Crashed too late.${NC}"
      return 1
  fi
  echo -e "${GREEN}$command OK.${NC}"
  #(echo "$output" | grep -E -- "$snippet") >/dev/null && echo -e "${GREEN}$command OK.${NC}" || echo -e "${RED}Test for $command failed.${NC}"
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

#get the absolute path of the base of spp project
SPPBASE=$(realpath "$(dirname "$0")")

#go to the root path
cd "$(dirname "$0")"

#force the user to give an optimization level to run the tests
OPT=0
if [ "$OPT_LEVEL" = "1" ]
then
NDEBUG=0
OPT=$OPT_LEVEL
# change the CMakeLists to use the appropriate optimization level
sed -i 's/-O2/-O'${OPT}'/g' tests/CMakeLists.txt
elif [ "$OPT_LEVEL" = "2" ]
then
NDEBUG=1
OPT=$OPT_LEVEL
# change the CMakeLists to use the appropriate optimization level
sed -i 's/-O1/-O'${OPT}'/g' tests/CMakeLists.txt
else
  echo "please set an optimization level with env var OPT_LEVEL"
  echo "e.g., OPT_LEVEL=1 ./spp_test.sh"
  exit
fi

mkdir -p build
cd build
[ -e Makefile ] && make clean

SPPLIBOBJ=${SPPBASE}/runtime/obj

WRAP_LIST="-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp \
           -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp \
           -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat \
           -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul \
           -Wl,-wrap,strcat -Wl,-wrap,snprintf \
           -Wl,-wrap,memcpy -Wl,-wrap,memset -Wl,-wrap,memmove \
           -Wl,-wrap,pmem_memmove_persist -Wl,-wrap,pmem_memcpy_persist \
           -Wl,-wrap,pmem_memmove_nodrain -Wl,-wrap,pmem_memcpy_nodrain \
           -Wl,-wrap,pmem_memmove -Wl,-wrap,pmem_memcpy \
           -Wl,-wrap,pmem_memset_nodrain -Wl,-wrap,pmem_memset \
           -Wl,-wrap,pmem_memset_persist -Wl,-wrap,pmemobj_memcpy \
           -Wl,-wrap,pmemobj_memcpy_persist -Wl,-wrap,pmemobj_memmove \
           -Wl,-wrap,pmemobj_memset -Wl,-wrap,pmemobj_memset_persist"

CFLAGS='-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -flto -O'${OPT}' -Xclang -load -Xclang '${SPPBASE}'/llvm-project/build/lib/LLVMSPP.so -include '${SPPBASE}'/runtime/src/spp.h'

CXXFLAGS='-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -flto -O'${OPT}' -Xclang -load -Xclang '${SPPBASE}'/llvm-project/build/lib/LLVMSPP.so -include '${SPPBASE}'/runtime/src/spp.h'

LDFLAGS='-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -fuse-ld=gold '${WRAP_LIST}' '${SPPBASE}'/runtime/obj/wrappers_spp.o -Xlinker '${SPPBASE}'/runtime/obj/spp.o'

echo "__CLANG:    $(which clang)"
echo "__CLANG++:  $(which clang++)"
echo "__GOLD:     $(which ld.gold)"
echo "__CXXFLAGS: ${CXXFLAGS}"
echo "__LDFLAGS:  ${LDFLAGS}"

if [ "$NDEBUG" = "1" ]
then
  cmake .. -DSPPBASE="${SPPBASE}" -DCMAKE_C_COMPILER=clang  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="${CXXFLAGS}" -DCMAKE_CFLAGS="${CFLAGS}" -DCMAKE_LDFLAGS="${LDFLAGS}" -DCMAKE_BUILD_TYPE=RelWithDebInfo 
  #cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
  echo "___N_Debug_________________"
else
  cmake ..  -DSPPBASE="${SPPBASE}" -DCMAKE_C_COMPILER=clang  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="${CXXFLAGS}" -DCMAKE_CFLAGS="${CFLAGS}" -DCMAKE_LDFLAGS="${LDFLAGS}" -DCMAKE_BUILD_TYPE=Debug 
  #cmake ..  -DCMAKE_BUILD_TYPE=Debug
  echo "___Debug___________________"
fi

make -j$(nproc) VERBOSE=1
# make VERBOSE=1

cd tests

set +e

should_crash "00\[fa\]" ./overflow
should_crash "00\[fa\]" ./type_overflow
should_crash "00\[fa\]" ./overflow_ntx
should_crash "00\[fa\]" ./root_overflow
should_not_crash ./root_underflow
should_crash "00\[fa\]" ../../tests/overflow_persistence.sh
should_not_crash ./free_tx_abort
should_crash "00\[fa\]" ./tx_add_overflow
should_not_crash ./zalloc
