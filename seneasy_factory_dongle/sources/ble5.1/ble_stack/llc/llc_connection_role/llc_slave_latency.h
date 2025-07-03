#ifndef LLC_SLAVE_LATENCY_H
#define LLC_SLAVE_LATENCY_H

void rom_llc_slave_latency_init(stLlcConnectionRole_t* pstRole);

void rom_llc_slave_latency_on_scheduler_event_prepare(stLlcConnectionRole_t* pstRole);

void rom_llc_slave_latency_on_conn_event_closed(stLlcConnectionRole_t* pstRole);

void rom_llc_slave_latency_on_tx_data_added(stLlcConnectionRole_t* pstRole);


#endif
