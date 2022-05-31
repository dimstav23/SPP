set -e

RED='\033[0;31m'
NC='\033[0m' # No Color

if [ "$#" -ne 2 ]
then
  echo -e "Usage: ${RED}plot_all.sh <pmembench results location> <pmemkv results location>${NC}" 2>&1
  echo -e "Using default result paths:"
  # exit 1
fi

PMEMBENCH_RES="${1:-"../benchmarks/pmembench/results"}"
PMEMKV_RES="${2:-"../benchmarks/pmemkv/results"}"

echo -e "pmembench result path: ${RED}$PMEMBENCH_RES${NC}"
echo -e "pmemkv result path: ${RED}$PMEMKV_RES${NC}"

bash import_pmembench.sh $PMEMBENCH_RES
bash import_pmemkv.sh $PMEMKV_RES

python3 generate_plots.py plot_config_maps.yml
python3 generate_plots.py plot_config_pmembench_tx.yml
#python3 generate_plots.py plot_config_partial_coverage.yml
#python3 generate_plots.py plot_config_open_create.yml
python3 generate_plots.py plot_config_pmemkv.yml

rm -rf pmdk spp safepm
