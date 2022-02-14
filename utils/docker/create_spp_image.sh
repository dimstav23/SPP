set -e

if [[ -z "${GIT_TOKEN}" ]]
then
	echo "Please export your github token in GIT_TOKEN environment variable." 2>&1
	exit 1
fi

docker build -t spp-llvm-clang-test -f=Dockerfile_spp --build-arg git_token=${GIT_TOKEN} .
