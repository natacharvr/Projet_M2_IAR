//--------------------------------------------------------------------------------
// Auto-generated by LiteX (3ab7eaa5) on 2024-10-23 13:56:15
//--------------------------------------------------------------------------------
#ifndef __GENERATED_SOC_H
#define __GENERATED_SOC_H
#define CONFIG_CLOCK_FREQUENCY 21000000
#define CONFIG_CPU_HAS_INTERRUPT
#define CONFIG_CPU_RESET_ADDR 0
#define CONFIG_CPU_HAS_ICACHE
#define CONFIG_CPU_TYPE_VEXRISCV
#define CONFIG_CPU_VARIANT_LITE
#define CONFIG_CPU_HUMAN_NAME "VexRiscv_Lite"
#define CONFIG_CPU_NOP "nop"
#define POGOBOT_VERSION "3"
#define SPIFLASH_PAGE_SIZE 256
#define SPIFLASH_SECTOR_SIZE 65536
#define RGB_LEDS 5
#define RGB_FIFO_SIZE 5
#define TX_FIFO_SIZE 512
#define RX_FIFO_SIZE 512
#define CONFIG_CSR_DATA_WIDTH 32
#define CONFIG_CSR_ALIGNMENT 32
#define CONFIG_BUS_STANDARD "WISHBONE"
#define CONFIG_BUS_DATA_WIDTH 32
#define CONFIG_BUS_ADDRESS_WIDTH 32
#define CONFIG_BUS_BURSTING 0
#define IR_RX0_INTERRUPT 4
#define IR_RX1_INTERRUPT 5
#define IR_RX2_INTERRUPT 6
#define IR_RX3_INTERRUPT 7
#define TIMER0_INTERRUPT 2
#define UART_INTERRUPT 0
#define CONFIG_BITSTREAM_SYNC_HEADER1 2123999870
#define CONFIG_BITSTREAM_SYNC_HEADER2 2125109630

#ifndef __ASSEMBLER__
static inline int config_clock_frequency_read(void) {
	return 21000000;
}
static inline int config_cpu_reset_addr_read(void) {
	return 0;
}
static inline const char * config_cpu_human_name_read(void) {
	return "VexRiscv_Lite";
}
static inline const char * config_cpu_nop_read(void) {
	return "nop";
}
static inline const char * pogobot_version_read(void) {
	return "3";
}
static inline int spiflash_page_size_read(void) {
	return 256;
}
static inline int spiflash_sector_size_read(void) {
	return 65536;
}
static inline int rgb_leds_read(void) {
	return 5;
}
static inline int rgb_fifo_size_read(void) {
	return 5;
}
static inline int tx_fifo_size_read(void) {
	return 512;
}
static inline int rx_fifo_size_read(void) {
	return 512;
}
static inline int config_csr_data_width_read(void) {
	return 32;
}
static inline int config_csr_alignment_read(void) {
	return 32;
}
static inline const char * config_bus_standard_read(void) {
	return "WISHBONE";
}
static inline int config_bus_data_width_read(void) {
	return 32;
}
static inline int config_bus_address_width_read(void) {
	return 32;
}
static inline int config_bus_bursting_read(void) {
	return 0;
}
static inline int ir_rx0_interrupt_read(void) {
	return 4;
}
static inline int ir_rx1_interrupt_read(void) {
	return 5;
}
static inline int ir_rx2_interrupt_read(void) {
	return 6;
}
static inline int ir_rx3_interrupt_read(void) {
	return 7;
}
static inline int timer0_interrupt_read(void) {
	return 2;
}
static inline int uart_interrupt_read(void) {
	return 0;
}
static inline int config_bitstream_sync_header1_read(void) {
	return 2123999870;
}
static inline int config_bitstream_sync_header2_read(void) {
	return 2125109630;
}
#endif // !__ASSEMBLER__

#endif
