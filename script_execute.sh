#!/bin/bash
output_dir="output"
input_dir="input_data"
binary="./CoperSim"

function exe_cmd() {
    binary=$1
    latency_file=$2
    trace_name=$3

    # Dense
    # config_dense="dense ${latency_file} ${input_dir}/${trace_name}"
    # config_name_dense=${trace_name}_${latency_file}_dense
    #
    # ${binary} ${config_dense} > ${output_dir}/${config_name_dense}.out

    # # LIL
    config_lil="lil ${latency_file} ${input_dir}/${trace_name}_LIL_INDICES ${input_dir}/${trace_name}_LIL_VALUES"
    config_name_lil=${trace_name}_${latency_file}_lil

    ${binary} ${config_lil} > ${output_dir}/${config_name_lil}.out

    # CSR
    config_csr="csr ${latency_file} ${input_dir}/${trace_name}_CSR_OFFSETS ${input_dir}/${trace_name}_CSR_INDICES ${input_dir}/${trace_name}_CSR_VALUES"
    config_name_csr=${trace_name}_${latency_file}_csr

    ${binary} ${config_csr} > ${output_dir}/${config_name_csr}.out

    # BCSR
    config_bcsr="bcsr ${latency_file} ${input_dir}/${trace_name}_BCSR_OFFSETS ${input_dir}/${trace_name}_BCSR_INDICES ${input_dir}/${trace_name}_BCSR_VALUES"
    config_name_bcsr=${trace_name}_${latency_file}_bcsr

    ${binary} ${config_bcsr} > ${output_dir}/${config_name_bcsr}.out
    
    # COO
    config_coo="coo ${latency_file} ${input_dir}/${trace_name}_COO_TUPLES"
    config_name_coo=${trace_name}_${latency_file}_coo

    ${binary} ${config_coo} > ${output_dir}/${config_name_coo}.out

    # CSC
    config_csc="csc ${latency_file} ${input_dir}/${trace_name}_CSC_OFFSETS ${input_dir}/${trace_name}_CSC_INDICES ${input_dir}/${trace_name}_CSC_VALUES"
    config_name_csc=${trace_name}_${latency_file}_csc

    ${binary} ${config_csc} > ${output_dir}/${config_name_csc}.out

    # DIA
    config_dia="dia ${latency_file} ${input_dir}/${trace_name}_DIA_DIAGONALS"
    config_name_dia=${trace_name}_${latency_file}_dia

    ${binary} ${config_dia} > ${output_dir}/${config_name_dia}.out

    # ELL
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
  for trace_name in "2cubes_sphere" "amazon0601" "dwt_918" "europe_osm" "flickr" "Freescale2" "hcircuit" "hugebubbles-00000" "kron_g500-logn21" "N_reactome" "patents" "rail582" "rajat31" "road_central" "roadNet-TX" "soc-LiveJournal1" "thermomech_dK" "wb-edu" "web-Google" "wiki-Talk" "wikipedia-20070206" "A_band_-1_1" "A_band_-2_2" "A_band_-4_4" "A_band_-8_8" "A_band_-16_16" "A_band_-32_32" "A_diagonal" "A_rand_0.1" "A_rand_0.2" "A_rand_0.3" "A_rand_0.4" "A_rand_0.5" "A_rand_0.6" "A_rand_0.01" "A_rand_0.001" "A_rand_0.0001"
  do
      echo $trace_name
      exe_cmd "${binary}" "$latency_file" "$trace_name"
  done
done
