set -e

if [[ -z "${GIT_TOKEN}" ]]
then
	echo "Please export your github token in GIT_TOKEN environment variable." 2>&1
	exit 1
fi

docker build -t llvm-clang-12-test -f=Dockerfile_clang_12 --build-arg git_token=${GIT_TOKEN} .
