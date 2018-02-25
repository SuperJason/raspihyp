#include <arch.h>
#include <assert.h>

/*******************************************************************************
 * Helper to create a level 1/2 table descriptor which points to a level 2/3
 * table.
 ******************************************************************************/
unsigned long create_table_desc(unsigned long *next_table_ptr)
{
	unsigned long desc = (unsigned long) next_table_ptr;

	/* Clear the last 12 bits */
	desc >>= FOUR_KB_SHIFT;
	desc <<= FOUR_KB_SHIFT;

	desc |= TABLE_DESC;

	return desc;
}

/*******************************************************************************
 * Helper to create a level 1/2/3 block descriptor which maps the va to addr
 ******************************************************************************/
unsigned long create_block_desc(unsigned long desc,
				unsigned long addr,
				unsigned int level)
{
	switch (level) {
	case LEVEL1:
		desc |= (addr << FIRST_LEVEL_DESC_N) | BLOCK_DESC;
		break;
	case LEVEL2:
		desc |= (addr << SECOND_LEVEL_DESC_N) | BLOCK_DESC;
		break;
	case LEVEL3:
		desc |= (addr << THIRD_LEVEL_DESC_N) | TABLE_DESC;
		break;
	default:
		assert(0);
	}

	return desc;
}

/*******************************************************************************
 * Helper to create a level 1/2/3 block descriptor which maps the va to output_
 * addr with Device nGnRE attributes.
 ******************************************************************************/
unsigned long create_device_block(unsigned long output_addr,
				  unsigned int level,
				  unsigned int ns)
{
	unsigned long upper_attrs, lower_attrs, desc;

	lower_attrs = LOWER_ATTRS(ACCESS_FLAG | OSH | AP_RW);
	lower_attrs |= LOWER_ATTRS(ns | ATTR_DEVICE_INDEX);
	upper_attrs = UPPER_ATTRS(XN);
	desc = upper_attrs | lower_attrs;

	return create_block_desc(desc, output_addr, level);
}

/*******************************************************************************
 * Helper to create a level 1/2/3 block descriptor which maps the va to output_
 * addr with inner-shareable normal wbwa read-only memory attributes.
 ******************************************************************************/
unsigned long create_romem_block(unsigned long output_addr,
				 unsigned int level,
				 unsigned int ns)
{
	unsigned long upper_attrs, lower_attrs, desc;

	lower_attrs = LOWER_ATTRS(ACCESS_FLAG | ISH | AP_RO);
	lower_attrs |= LOWER_ATTRS(ns | ATTR_IWBWA_OWBWA_NTR_INDEX);
	upper_attrs = UPPER_ATTRS(0ull);
	desc = upper_attrs | lower_attrs;

	return create_block_desc(desc, output_addr, level);
}

/*******************************************************************************
 * Helper to create a level 1/2/3 block descriptor which maps the va to output_
 * addr with inner-shareable normal wbwa read-write memory attributes.
 ******************************************************************************/
unsigned long create_rwmem_block(unsigned long output_addr,
				 unsigned int level,
				 unsigned int ns)
{
	unsigned long upper_attrs, lower_attrs, desc;

	lower_attrs = LOWER_ATTRS(ACCESS_FLAG | ISH | AP_RW);
	lower_attrs |= LOWER_ATTRS(ns | ATTR_IWBWA_OWBWA_NTR_INDEX);
	upper_attrs = UPPER_ATTRS(XN);
	desc = upper_attrs | lower_attrs;

	return create_block_desc(desc, output_addr, level);
}
