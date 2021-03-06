SysHashTable *asm_hash;
BoolI8 is_first_compiler,cmp_do_not_write_to_dsk;
ClassStruct *sys_internal_types[IT_NUM_IT];

U32 *dual_U16_tokens1,*dual_U16_tokens2,*dual_U16_tokens3;
U32 *binary_ops_table,*unary_ops_table;
U8 *cmp_arg_cnt,*cmp_result_cnt;
U8 *deref_bitmap,*assign_bitmap,*not_constant_bitmap,*cmp_bitmap,*fpop_bitmap;
I64 num_stk_temps,num_reg_vars,num_non_ptr_vars;
U64 stk_temps_mask,reg_vars_mask,non_ptr_vars_mask;
U8 *to_stk_temps_map_odd;
U8 *to_stk_temps_map_even;
U8 *to_reg_vars_map,*to_non_ptr_vars_map;
U64 compiled_lines,compiled_funs,compiled_bytes;

//Unassembler
InstructionEntry **unasm_table;
I64 unasm_table_entries;
U64 uasm_ins64_mask,uasm_signed_arg_mask,uasm_mem_mask;

