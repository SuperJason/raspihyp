#include <assert.h>
#include <context.h>
#include <errno.h>
#include <interrupt.h>
#include <stdio.h>

/*******************************************************************************
 * Local structure and corresponding array to keep track of the state of the
 * registered interrupt handlers for each interrupt type.
 * The field descriptions are:
 *
 * 'flags' : Bit[0], Routing model for this interrupt type when execution is
 *                   not in EL2. '1' implies that this
 *                   interrupt will be routed to EL3. '0' implies that this
 *                   interrupt will be routed to the current exception level.
 *
 *           All other bits are reserved and SBZ.
 *
 * 'hcr_el2'  : Mapping of the routing model in the 'flags' field to the
 *              Virtial IRQ or FIQ. TODO
 ******************************************************************************/
typedef struct intr_type_desc {
	interrupt_type_handler_t handler;
	uint32_t flags;
	uint32_t hcr_el2;
} intr_type_desc_t;

static intr_type_desc_t intr_type_descs[MAX_INTR_TYPES];

/*******************************************************************************
 * This function validates the interrupt type. EL3 interrupts are currently not
 * supported.
 ******************************************************************************/
static int32_t validate_interrupt_type(uint32_t type)
{
	if (type == INTR_TYPE_EL2)
		return -ENOTSUP;

	if (type != INTR_TYPE_S_EL1 && type != INTR_TYPE_NS)
		return -EINVAL;

	return 0;
}

/*******************************************************************************
 * This function validates the routing model specified in the 'flags' and
 * updates internal data structures to reflect the new routing model. It also
 * updates the copy of SCR_EL3 for each security state with the new routing
 * model in the 'cpu_context' structure for this cpu.
 ******************************************************************************/
int32_t set_routing_model(uint32_t type, uint32_t flags)
{
	int32_t rc;

	rc = validate_interrupt_type(type);
	if (rc)
		return rc;

	/* Update the routing model in internal data structures */
	intr_type_descs[type].flags = flags;

	return 0;
}

/*******************************************************************************
 * This function registers a handler for the 'type' of interrupt specified. It
 * also validates the routing model specified in the 'flags' for this type of
 * interrupt.
 ******************************************************************************/
int32_t register_interrupt_type_handler(uint32_t type,
					interrupt_type_handler_t handler,
					uint32_t flags)
{
	int32_t rc;

	/* Validate the 'handler' parameter */
	if (!handler)
		return -EINVAL;

	/* Validate the 'flags' parameter */
	if (flags & INTR_TYPE_FLAGS_MASK)
		return -EINVAL;

	/* Check if a handler has already been registered */
	if (intr_type_descs[type].handler)
		return -EALREADY;

	rc = set_routing_model(type, flags);
	if (rc)
		return rc;

	/* Save the handler */
	intr_type_descs[type].handler = handler;

	return 0;
}

/*******************************************************************************
 * This function is called when an interrupt is generated and returns the
 * handler for the interrupt type (if registered). It returns NULL if the
 * interrupt type is not supported or its handler has not been registered.
 ******************************************************************************/
interrupt_type_handler_t get_interrupt_type_handler(uint32_t type)
{
	if (validate_interrupt_type(type))
		return NULL;

	return intr_type_descs[type].handler;
}

