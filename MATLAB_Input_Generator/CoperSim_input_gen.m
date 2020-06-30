
file_dir = dir('../raw_matrices/*.mat');
read_dir = '../raw_matrices/';
dir_name = '../inputs_to_CoperSim/';

for k=1:length(file_dir)
    file_name = file_dir(k).name;
    file_name;        

        % ------ output (input to CoperSim) files:
        % 0. DENSE
        DENSE = fopen(strcat(dir_name,erase(file_name,".mat"),'_DENSE'),'w');
        
        % 1. LIL
        LIL_indices = fopen(strcat(dir_name,erase(file_name,".mat"),'_LIL_INDICES'),'w');
        LIL_values = fopen(strcat(dir_name,erase(file_name,".mat"),'_LIL_VALUES'),'w');
        
        % 2. CSR
        CSR_offsets = fopen(strcat(dir_name,erase(file_name,".mat"),'_CSR_OFFSETS'),'w');
        CSR_col_indices = fopen(strcat(dir_name,erase(file_name,".mat"),'_CSR_COL_INDICES'),'w');
        CSR_values = fopen(strcat(dir_name,erase(file_name,".mat"),'_CSR_VALUES'),'w');
        
        % 3. BCSR
        BCSR_offsets = fopen(strcat(dir_name,erase(file_name,".mat"),'_BCSR_OFFSETS'),'w');
        BCSR_col_indices = fopen(strcat(dir_name,erase(file_name,".mat"),'_BCSR_COL_INDICES'),'w');
        BCSR_values = fopen(strcat(dir_name,erase(file_name,".mat"),'_BCSR_VALUES'),'w');
        
        % 4. COO
        COO_tuples = fopen(strcat(dir_name,erase(file_name,".mat"),'_COO_TUPLES'),'w');
        
        % 5. CSC
        CSC_offsets = fopen(strcat(dir_name,erase(file_name,".mat"),'_CSC_OFFSETS'),'w');
        CSC_row_indices = fopen(strcat(dir_name,erase(file_name,".mat"),'_CSC_ROW_INDICES'),'w');
        CSC_values = fopen(strcat(dir_name,erase(file_name,".mat"),'_CSC_VALUES'),'w');
        
        % 6. DIA
        DIA_diagonals = fopen(strcat(dir_name,erase(file_name,".mat"),'_DIA_DIAGONALS'),'w');
        
        % 7. ELL
        ELL_indices = fopen(strcat(dir_name,erase(file_name,".mat"),'_ELL_INDICES'),'w');
        ELL_values = fopen(strcat(dir_name,erase(file_name,".mat"),'_ELL_VALUES'),'w');
        
        stats = fopen(strcat(dir_name,erase(file_name,".mat"),'_stat'),'w');

        Operation = 0;
        Operand_vector = 0;
        Input_index = 0;
        Access_order = 0;
        Output_index = 0;
        Memory_address = 0;
        
        partition_width = 8000;
        block_width = 8;
        block_width_small = 4; % for BCSR
        ell_width = 6; % for ELL
        p_i=1;
        p_j=1;
        w=0;
        h=0;
        bd=1;
             
        % partitioning the large matrix into small ones
        Data = load(strcat(read_dir,file_name));
        A = Data.Problem.A;
        A_size = size(A); % a 2x2 matrix including the dimension of A
        partitions = round((A_size/partition_width))+1;  % a 2x2 matrix including the number of partitions at each dimension
                
        % parameters for statistics:
        block_density = zeros(A_size(1),1);
        total_nz_row = 0;
        total_nz_row_per_block = 0;
        total_nz_blocks = 0;
        total_nz = 0;
        max_nz_row = 0;
        total_nz_per_row = 0;
        total_nz_per_block = 0;
        
       
        for p = 1:(partitions(1)*partitions(2)) % this iterates over all 8000x8000 partitions
%         for p = 1:1 % we may want to process only a few partitions as
            p;
            if ((p_i+partition_width) > A_size(1))
                w = A_size(1);
            else
                w = p_i+partition_width-1;
            end

            if ((p_j+partition_width) > A_size(1))
                h = A_size(1);
            else
                h = p_i+partition_width-1;
            end

            partition = A(p_i:w,p_j:h); % this is a 8000 x 8000 partition that we are going to process

            if (w == A_size(1))
                p_i = 1;
            else
                p_i = w+1;
            end

            if (h == A_size(1))
                p_j = 1;
            else 
                p_j = h+1;
            end


            partition_size = size(partition); % 8000 x 8000
            blocks = round((partition_size/block_width))+1; 
            spliter = zeros(blocks(1),1);
            pad = (blocks(1)*block_width - partition_size(1));
            partition_pad = padarray(partition,[pad pad],0,'pre');
            spliter(:) = block_width;
            partition_pad_size = size(partition_pad);
            C = mat2cell(partition_pad, spliter, spliter); % we break down a partition into 8x8 blocks
            size_C = size(C); % this is the number of blocks which is 1000 x 1000 


            zero_cells = 0;

            % iterate over all 8x8 blocks
            for i = 1:size_C(1)
                for j = 1:size_C(2)
                    if (nnz(C{i,j})> 0) % if a block is not zero                     
                        
                        % calculate some statistics and write them down in stat file:
                        % fprintf(stats,'%d %d %d %d %d\n', p_i, p_j, i, j, nnz(C{i,j}));
                         block_density(bd)=nnz(C{i,j});
                         total_nnz_blocks = total_nnz_blocks + 1;
                         total_nnz = total_nnz + nnz(C{i,j});
                         for ii = 1:block_width
                             if(nnz(C{i,j}(ii,:) > 0))
                                 total_nz_row = total_nz_row + 1;
                                 total_nz_row_per_block = total_nz_row_per_block + 1;
                                 total_nz_per_row = total_nz_per_row + nnz(C{i,j}(ii,:));
                                 total_nz_per_block = total_nz_per_block + nnz(C{i,j});
                             end
                         end
                         if (total_nz_row_per_block > max_nz_row)
                             max_nz_row = total_nz_row_per_block;
                         end
                         total_nz_row_per_block = 0;
                        
                        % create CoperSim input files by compressing a
                        % block (C{i,j}) into different formats. one block
                        % will be writen as a single line in the
                        % correspoinding file(s)
                        
                        % 0. DENSE:
                        for ii = 1:block_width
                            for jj = 1:block_width
                                if (C{i,j}(ii,jj) == 0)
                                    fprintf(DENSE,'%d ', 0);
                                elseif (C{i,j}(ii,jj) ~= 0)
                                    fprintf(DENSE,'%d ', 1);
                                end
                            end
                        end
                        fprintf(DENSE,'\n');
                        
                        % 1. LIL
                        inx = ones(block_width,1);
                        % first push all nz values to top in in the tmps
                        tmp_vals = zeros(block_width, block_width);
                        tmp_inxs = zeros(block_width, block_width);
                        for ii = 1:block_width
                            for jj = 1:block_width
                                if (C{i,j}(ii,jj) ~= 0)
                                    tmp_vals(inx(jj),jj) = 1; %C{i,j}(ii,jj);
                                    tmp_inxs(inx(jj),jj) = ii;
                                    inx(jj) = inx(jj) + 1;
                                end
                            end
                        end
                        % then write the tmps (indices and values) in files
                        for ii = 1:block_width
                            if (nnz(tmp_inxs(ii,:) > 0)) % only if a row is not zero
                                for jj = 1:block_width
                                    fprintf(LIL_values,'%d ', tmp_vals(ii,jj));
                                    fprintf(LIL_indices,'%d ', tmp_inxs(ii,jj));
                                end
                            end
                        end
                        fprintf(LIL_indices,'\n');
                        fprintf(LIL_values,'\n');
                        
                        % 2. CSR
                        offset = 0;
                        for ii = 1:block_width
                            offset = offset + nnz(C{i,j}(ii,:)); 
                            fprintf(CSR_offsets,'%d ', offset); % number of nz values in a row
                            for jj = 1:block_width
                                 if (C{i,j}(ii,jj) ~= 0) % write all the nz values and corresponding col indices per each row
                                    fprintf(CSR_col_indices,'%d ', jj); % jj is the col index of a value
                                    fprintf(CSR_values,'%d ', 1); % C{i,j}(ii,jj)); % writie the value itself
                                 end
                            end
                        end
                        fprintf(CSR_offsets,'\n');
                        fprintf(CSR_col_indices,'\n');
                        fprintf(CSR_values,'\n');
                        
                        % 3. BCSR
                        num_row_blocks = round((block_width/block_width_small)); 
                        row_spliter = zeros(num_row_blocks,1);
                        row_spliter(:) = block_width_small;
                        num_col_blocks = round(block_width/block_width_small); 
                        col_spliter = zeros(num_col_blocks,1);
                        col_spliter(:) = block_width_small;
                        
                        B = mat2cell(C{i,j}, row_spliter, col_spliter); % breaking down a block into 4x4 sub blocks
                        nnz_block_per_row = 0;
                        offset = 0;
                        for ii = 1:num_row_blocks
                            for jj = 1:num_col_blocks
                                if (nnz(B{ii,jj}) > 0)
                                    nnz_block_per_row = nnz_block_per_row + 1;
                                    tmp_block = reshape(B{ii,jj},[block_width_small * block_width_small 1]);
                                    for kk = 1:(block_width_small * block_width_small)
                                        if (tmp_block(kk,1) == 0)
                                            fprintf(BCSR_values,'%d ', 0); 
                                        elseif (tmp_block(kk,1) ~= 0)
                                            fprintf(BCSR_values,'%d ', 1);
                                        end
                                    end
                                    fprintf(BCSR_col_indices,'%d ', (jj-1)* block_width_small); % the column index of the begining of a sub block
                                end
                            end
                            offset = offset + nnz_block_per_row;
                            fprintf(BCSR_offsets,'%d ', offset);
                            nnz_block_per_row = 0;
                        end
                        fprintf(BCSR_offsets,'\n');
                        fprintf(BCSR_col_indices,'\n');
                        fprintf(BCSR_values,'\n');
                        
                        % 4. COO
                        for ii = 1:block_width
                            for jj = 1:block_width
                                 if (C{i,j}(ii,jj) ~= 0) % write all the nz values and corresponding row, col indices 
                                    fprintf(COO_tuples,'%d %d %d ', ii, jj, 1); % C{i,j}(ii,jj));
                                 end
                            end
                        end
                        fprintf(COO_tuples,'\n');
                        
                        % 5. CSC
                        offset = 0;
                        for jj = 1:block_width
                            offset = offset + nnz(C{i,j}(:,jj)); 
                            fprintf(CSC_offsets,'%d ', offset); % number of nz values in a col
                            for ii = 1:block_width
                                 if (C{i,j}(ii,jj) ~= 0) % write all the nz values and corresponding row indices per each col
                                    fprintf(CSC_row_indices,'%d ', ii); % jj is the col index of a value
                                    fprintf(CSC_values,'%d ', 1); % C{i,j}(ii,jj)); % writie the value itself
                                 end
                            end
                        end
                        fprintf(CSC_offsets,'\n');
                        fprintf(CSC_row_indices,'\n');
                        fprintf(CSC_values,'\n');
                        
                        % 6. DIA                      
                        for ii = 1-block_width:1:block_width-1
                            if (nnz(diag(C{i,j},ii))>0)
                                temp_dia = diag(C{i,j},ii);
                                fprintf(DIA_diagonals,'%d ', ii);
                                for jj = 1:length(temp_dia)
                                    if (temp_dia(jj) == 0)
                                        fprintf(DIA_diagonals,'%d ', 0); %temp_dia(jj));
                                    elseif (temp_dia(jj) ~= 0)
                                        fprintf(DIA_diagonals,'%d ', 1); %temp_dia(jj));
                                    end
                                end
                                for kk = jj+1:block_width % fill the rest with 100
                                    fprintf(DIA_diagonals,'%d ', 100);
                                end
                            end
                        end
                        fprintf(DIA_diagonals,'\n');
                        
                        % 7. ELL
                        inx = ones(block_width,1);
                        % first push all nz values to top in in the tmps
                        tmp_vals = zeros(block_width, block_width);
                        tmp_inxs = zeros(block_width, block_width);
                        for jj = 1:block_width
                            for ii = 1:block_width
                                if (C{i,j}(ii,jj) ~= 0)
                                    tmp_vals(ii,inx(ii)) = 1; %C{i,j}(ii,jj);
                                    tmp_inxs(ii,inx(ii)) = jj;
                                    inx(ii) = inx(ii) + 1;
                                end
                            end
                        end
                        % then write the tmps (indices and values) in files
                        for ii = 1:block_width
                            for jj = 1:ell_width
                                fprintf(ELL_values,'%d ', tmp_vals(ii,jj));
                                fprintf(ELL_indices,'%d ', tmp_inxs(ii,jj));
                            end
                        end
                        fprintf(ELL_indices,'\n');
                        fprintf(ELL_values,'\n');
                        
                    end
                end
            end
            
        end

        fclose(DENSE);
        fclose(LIL_indices);
        fclose(LIL_values);
        fclose(CSR_offsets);
        fclose(CSR_col_indices);
        fclose(CSR_values);
        fclose(BCSR_offsets);
        fclose(BCSR_col_indices);
        fclose(BCSR_values);
        fclose(COO_tuples);
        fclose(CSC_offsets);
        fclose(CSC_row_indices);
        fclose(CSC_values);
        fclose(DIA_diagonals);
        fclose(ELL_indices);
        fclose(ELL_values);
        fclose(stats);
        
        fprintf(stats,'total_nnz_blocks %d\n', total_nz_blocks);
        fprintf(stats,'total_nnz_rows %d\n', total_nz_row);
        
        avg_nz_per_block = total_nz_per_block / total_nz_blocks;
        fprintf(stats,'avg_nz_per_block %d\n', avg_nz_per_block);
        
        avg_nz_per_row = total_nz_per_row / total_nz_row;
        fprintf(stats,'avg_nz_per_row %d\n', avg_nz_per_row);
        
        mean_density = 100 * mean(block_density)/(block_width*block_width);
        fprintf(stats,'mean_density %d\n', mean_density);
        
        avg_nz_row =  total_nz_row / total_nz_blocks;
        fprintf(stats,'mean_nz_row_per_nnz_blocks %d\n', avg_nz_row);
        
        fprintf(stats,'max_nz_row %d\n', max_nnz_row);
        
        avg_nz_per_nnz_row = total_nnz / total_nnz_row;
        fprintf(stats,'avg_nz_per_nnz_row %d\n', avg_nz_per_nnz_row);
        
end

