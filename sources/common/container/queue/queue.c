/**
 * @file queue.c
 */
#include "queue.h"

uint8_t queue_init(queue_t* q, uint32_t b, uint32_t q_l, uint32_t e_l, q_ctrl f_w, q_ctrl f_r) {
	q->base		= b;
	q->head		= b;
	q->tail		= b;

	q->q_len	= q_l;
	q->e_len	= e_l;

	q->counter	= 0;
	q->end		= b + ((q_l - 1) * e_l);

	if (f_w) {
		q->write = f_w;
	}
	else {
		return QUEUE_RET_NG;
	}

	if (f_r) {
		q->read = f_r;
	}
	else {
		return QUEUE_RET_NG;
	}

	return QUEUE_RET_OK;
}

uint32_t queue_len(queue_t* q) {
	return q->counter;
}

uint8_t queue_put(queue_t* q, void* d) {
	if ((q->head < q->base) || (q->head > q->end)) {
		return QUEUE_RET_NG;
	}

	if (q->write) {
		q->write(q->head, (uint8_t*)d, q->e_len);
	}

	if (q->tail == q->head && q->counter == q->q_len) {
		if (q->tail == q->end) {
			q->tail = q->base;
		}
		else {
			q->tail = q->tail + q->e_len;
		}
	}

	if (q->head == q->end) {
		q->head = q->base;
	}
	else {
		q->head = q->head + q->e_len;
	}

	q->counter++;
	if (q->counter > q->q_len) {
		q->counter = q->q_len;
	}

	return QUEUE_RET_OK;
}

uint8_t queue_get(queue_t* q, void* d) {
	if (!q->counter) {
		return QUEUE_RET_NG;
	}

	if (q->read) {
		q->read(q->tail, (uint8_t*)d, q->e_len);
	}
	if (q->tail == q->end) {
		q->tail = q->base;
	}
	else {
		q->tail = q->tail + q->e_len;
	}

	q->counter--;

	return QUEUE_RET_OK;
}

