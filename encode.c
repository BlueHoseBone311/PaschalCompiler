 /*@Title   encode.c - size calculations and code generating functtions
 * @authors  Venugopal Boppa, Christopher A. Greer, Christian Merchant
 * @class   CSCE531
 * @Project Pascal Compiler Part II
 * @date    04-17-15
*/

#include <stdio.h>
#include <stdlib.h>
#include "encode.h"


static int calc_array_size(TYPE array_type, int align);
static TYPE get_array_align_element(TYPE array);
static TYPE get_subrange_align_element(TYPE query_type);



void encode_declaration (TYPE type, VAR_ID_LIST varlist_id)
{

	if (ty_query(type)==TYERROR)
	{
		return;
	}
	if (ty_query(type)==TYFUNC)
	{
		return;
	}

	while (varlist_id)
	{
		ST_ID id = varlist_id->id;
		unsigned int size;
		char *decl_id;
		int align;

		TYPETAG tag;
		TYPE base_type;

		if (!id)
		{
			bug("Oops! ST_ID Received is Null\n");
		}

		tag = ty_query(type);
		if(tag == TYARRAY)
		{
            base_type = get_array_align_element(type);
			align = encode_get_base_size(base_type);
			size = calc_array_size(type, align);
		}
		else if (tag == TYSUBRANGE)
		{
			base_type = get_subrange_align_element(type);
			align = encode_get_base_size(base_type);
			size = align;
		}
		else
		{
			align = encode_get_base_size(type);
			size = align;
		}

		decl_id=st_get_id_str(id);

		b_global_decl(decl_id, align, size);

		switch (tag)
		{
			case TYARRAY:
				b_skip(size);
				break;
			case TYSUBRANGE:
				b_skip(size);
				break;
			case TYFLOAT:
				b_skip(size);
				break;
			case TYDOUBLE:
				b_skip(size);
				break;
			case TYLONGDOUBLE:
				b_skip(size);
				break;
			case TYUNSIGNEDINT:
				b_skip(size);
				break;
			case TYUNSIGNEDCHAR:
				b_skip(size);
				break;
			case TYUNSIGNEDSHORTINT:
				b_skip(size);
				break;
			case TYUNSIGNEDLONGINT:
				b_skip(size);
				break;
			case TYSIGNEDCHAR:
				b_skip(size);
				break;
			case TYSIGNEDINT:
				b_skip(size);
				break;
			case TYSIGNEDLONGINT:
				b_skip(size);
				break;
			case TYSIGNEDSHORTINT:
				b_skip(size);
				break;
			case TYPTR:
				b_skip(size);
				break;
			case TYSET:
				bug("Typetag ERROR  (%d)", tag);
				break;
			case TYSTRUCT:
				bug("Typetag ERROR (%d)", tag);
				break;
			case TYUNION:
				bug("Typetag ERROR (%d)", tag);
				break;
			case TYENUM:
				bug("Typetag ERROR (%d)", tag);
				break;
			case TYFUNC:
				break;
			case TYVOID:
				msgn("Type: Void");
				break;
			case TYERROR:
				msgn("Type: Error");
				break;
			default:
				bug(" Typetag error - Hit Default Action in (%d) ", tag);
		}

		varlist_id = varlist_id->next;
	}
}

/*Allocates space for basic types */
unsigned int encode_get_base_size (TYPE type)
{
	TYPETAG tp_tag;
	unsigned int bit_length;
	tp_tag = ty_query(type);

	switch (tp_tag)
	{
		case TYFLOAT:
			bit_length = 4;
			break;
		case TYDOUBLE:
			bit_length = 8;
			break;
		case TYLONGDOUBLE:
			bit_length = 8;
			break;
		case TYUNSIGNEDINT:
			bit_length = 4;
			break;
		case TYUNSIGNEDCHAR:
			bit_length = 1;
			break;
		case TYUNSIGNEDSHORTINT:
			bit_length = 2;
			break;
		case TYUNSIGNEDLONGINT:
			bit_length = 4;
			break;
		case TYSIGNEDCHAR:
			bit_length = 1;
			break;
		case TYSIGNEDINT:
			bit_length = 4;
			break;
		case TYSIGNEDLONGINT:
			bit_length = 4;
			break;
		case TYSIGNEDSHORTINT:
			bit_length = 2;
			break;
		case TYPTR:
			bit_length = 4;
			break;
		case TYARRAY:
			bug("encountered array typetag in encode_get_base_size");
			break;
		case TYSET:
			bug("illegal typetag (%d) in \"encode_get_base_size\"", tp_tag);
			break;
		case TYSTRUCT:
			bug("illegal typetag (%d) in \"encode_get_base_size\"", tp_tag);
			break;
		case TYUNION:
			bug("illegal typetag (%d) in \"encode_get_base_size\"", tp_tag);
			break;
		case TYENUM:
			bug("illegal typetag (%d) in \"encode_get_base_size\"", tp_tag);
			break;
		case TYFUNC:
			break;
		case TYSUBRANGE:
			bug("encountered subrange typetag in encode_get_base_size");
			break;
		case TYVOID:
			break;
		case TYERROR:
			break;
		default:
			bug(" Typetag error - Hit Default Action in (%d) ", tp_tag);
	}
	return bit_length;
}

/* STATIC SUBROUTINES */

static int calc_array_size(TYPE array_type, int align)
{
	long low;
	long high;
	TYPE indc_type;
	TYPE type;
	unsigned int align_size = align;
	INDEX_LIST indices;

	type = array_type;

	while (ty_query(type) == TYARRAY)
	{
		type = ty_query_array(type, &indices);
		while (indices != NULL)
		{
			indc_type = ty_query_subrange(indices->type,&low,&high);
			align_size *= high - low + 1;
			indices = indices->next;
		}
	}
	return align_size;
}

static TYPE get_array_align_element(TYPE query_type)
{
	TYPE new_type_query;
	INDEX_LIST ilist;
	if (ty_query(query_type) != TYARRAY)
	{
		return query_type;
	}

	 new_type_query = ty_query_array(query_type, &ilist);
	 get_array_align_element(new_type_query);

}

static TYPE get_subrange_align_element(TYPE query_type)
{
	TYPE new_type_query;
	if (ty_query(query_type) != TYSUBRANGE)
	{
		return query_type;
	}

	new_type_query = ty_strip_modifier(query_type);
	get_array_align_element(new_type_query);
}


//Project II

int get_formal_param_offset(TYPETAG tag)
{
  return b_get_formal_param_offset(tag);
}

int getSize(TYPE type) 
{
   int size = encode_get_base_size(type);
   return size;
}

void begin_funct_body(char *global_funct_name, TYPE type, int local_var_offset) 
{
   TYPE funct_type;
   TYPE param_type;
   PARAM_LIST param_list;
   BOOLEAN check;
   TYPETAG funct_tag;
   TYPETAG param_tag;

   int block;
   ST_DR data_record;
   long low,high;

   //query the function to get variables
   funct_type = ty_query_funct(type, &param_list, &check);
   funct_tag = ty_query(funct_type);

   

   while (param_list != NULL) 
   {
      param_tag = ty_query(param_list->type);
      data_record = st_lookup(param_list->id, &block);

      /*Subrange? Use Base Type*/
      if (param_tag == TYSUBRANGE) 
      { 
         param_type = ty_query_subrange(type, &low, &high);
         b_store_formal_param(ty_query(param_type));
      }
      else if (data_record->u.decl.is_ref == TRUE) { //VAR param_list
         b_store_formal_param(TYPTR);
      } 
      else 
      {
         b_store_formal_param(param_tag);
      }

      param_list = param_list->next;
   }

   //nonvoid pascal function types
   if (func_tag != TYVOID) 
   {
      b_alloc_return_value();
   }

   //local variables
   b_alloc_local_vars(local_var_offset - b_get_local_var_offset());
}

void end_funct_body(char *global_funct_name, TYPE type) 
{
   TYPE funct_type;
   PARAM_LIST params;
   BOOLEAN funct_arg_flag;
   TYPETAG funct_tag;
   long low,high;

   //query function
   funct_type = ty_query_func(type, &params, &funct_arg_flag);
   funct_tag = ty_query(funct_type);
   
   //pops the function id from the global stack;
   func_top--;

   //pascal function (nonvoid types)
   if (funct_tag != TYVOID) 
   {
      if (funct_tag == TYSUBRANGE) 
      {
         b_prepare_return(ty_query(ty_query_subrange(funct_type, &low, &high)));
      }
      else 
      {
         b_prepare_return(funct_tag);
      }
   }

   b_func_epilogue(global_funct_name);
   st_exit_block();
}


