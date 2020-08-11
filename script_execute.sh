#!/bin/bash
# output_dir="output_synthetic_1000"
# output_dir="output_synthetic_8000"
output_dir="output_SuiteSparse"
# input_dir="inputs_synthetic_1000"
# input_dir="inputs_synthetic_8000"
input_dir="inputs_SuiteSparse"
binary="./CoperSim"

function exe_cmd() {
    binary=$1
    latency_file=$2
    trace_name=$3

    # Dense
    echo "DENSE"
    config_dense="dense ${latency_file} ${input_dir}/${trace_name}_DENSE"
    config_name_dense=${trace_name}_${latency_file}_dense

    ${binary} ${config_dense} > ${output_dir}/${config_name_dense}.out

    # # LIL
    echo "LIL"
    config_lil="lil ${latency_file} ${input_dir}/${trace_name}_LIL_INDICES ${input_dir}/${trace_name}_LIL_VALUES"
    config_name_lil=${trace_name}_${latency_file}_lil

    ${binary} ${config_lil} > ${output_dir}/${config_name_lil}.out

    # CSR
    echo "CSR"
    config_csr="csr ${latency_file} ${input_dir}/${trace_name}_CSR_OFFSETS ${input_dir}/${trace_name}_CSR_COL_INDICES ${input_dir}/${trace_name}_CSR_VALUES"
    config_name_csr=${trace_name}_${latency_file}_csr

    ${binary} ${config_csr} > ${output_dir}/${config_name_csr}.out

    # BCSR
    echo "BCSR"
    config_bcsr="bcsr ${latency_file} ${input_dir}/${trace_name}_BCSR_OFFSETS ${input_dir}/${trace_name}_BCSR_COL_INDICES ${input_dir}/${trace_name}_BCSR_VALUES"
    config_name_bcsr=${trace_name}_${latency_file}_bcsr

    ${binary} ${config_bcsr} > ${output_dir}/${config_name_bcsr}.out

    # COO
    echo "COO"
    config_coo="coo ${latency_file} ${input_dir}/${trace_name}_COO_TUPLES"
    config_name_coo=${trace_name}_${latency_file}_coo

    ${binary} ${config_coo} > ${output_dir}/${config_name_coo}.out

    # CSC
    echo "CSC"
    config_csc="csc ${latency_file} ${input_dir}/${trace_name}_CSC_OFFSETS ${input_dir}/${trace_name}_CSC_ROW_INDICES ${input_dir}/${trace_name}_CSC_VALUES"
    config_name_csc=${trace_name}_${latency_file}_csc

    ${binary} ${config_csc} > ${output_dir}/${config_name_csc}.out

    # DIA
    echo "DIA"
    config_dia="dia ${latency_file} ${input_dir}/${trace_name}_DIA_DIAGONALS"
    config_name_dia=${trace_name}_${latency_file}_dia

    ${binary} ${config_dia} > ${output_dir}/${config_name_dia}.out

    # ELL
    echo "ELL"
    config_ell="ell ${latency_file} ${input_dir}/${trace_name}_ELL_INDICES ${input_dir}/${trace_name}_ELL_VALUES"
    config_name_ell=${trace_name}_${latency_file}_ell

    ${binary} ${config_ell} > ${output_dir}/${config_name_ell}.out
}

if [ ! -f "${binary}" ]
then
    echo "Executable ${binary}, do make first" not found
    exit 1
fi

rm -f -r ./${output_dir}
mkdir ./${output_dir}

for latency_file in "input_latency"
do
  echo "SuiteSparse dataset"
  for trace_name in "2cubes_sphere" "amazon0601" "dwt_918" "europe_osm" "flickr" "Freescale2" "hcircuit" "hugebubbles-00000" "kron_g500-logn21" "N_reactome" "rail582" "rajat31" "road_central" "roadNet-TX" "soc-LiveJournal1" "thermomech_dK" "wb-edu" "web-Google" "wiki-Talk" "wikipedia-20070206"

  # echo "synthetics 8000 dataset"
  # for trace_name in "A_band_-1_1" "A_band_-2_2" "A_band_-4_4" "A_band_-8_8" "A_band_-16_16" "A_band_-32_32" "A_diagonal" "A_rand_0.1" "A_rand_0.01" "A_rand_0.001" "A_rand_0.0001"

  # echo "synthetics 1000 dataset"
  # for trace_name in "A_band_-1_1" "A_band_-2_2" "A_band_-4_4" "A_band_-8_8" "A_band_-16_16" "A_band_-32_32" "A_diagonal" "A_rand_0.1" "A_rand_0.01" "A_rand_0.001" "A_rand_0.0001" "A_rand_0.00001" "A_rand_0.000001"

  do
      echo $trace_name
      exe_cmd "${binary}" "$latency_file" "$trace_name"
  done
done
