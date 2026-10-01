// lwipopts.h
// configuración de lwIP para SiguesAhi con el pico sdk.
// basada en pico-examples/pico_w/wifi (lwipopts_examples_common.h y
// tls_client/lwipopts.h), más SNTP para la hora.
// si tu programa necesita su propio lwipopts.h, desactiva
// SIGUES_AHI_CONFIGURACION en CMake y copia aquí lo marcado "SiguesAhi"

#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

#include <stdint.h>

#define NO_SYS 1
#define LWIP_SOCKET 0
#if PICO_CYW43_ARCH_POLL
#define MEM_LIBC_MALLOC 1
#else
#define MEM_LIBC_MALLOC 0
#endif
#define MEM_ALIGNMENT 4
#define MEM_SIZE 8000
#define MEMP_NUM_TCP_SEG 32
#define MEMP_NUM_ARP_QUEUE 10
#define PBUF_POOL_SIZE 24
#define LWIP_ARP 1
#define LWIP_ETHERNET 1
#define LWIP_ICMP 1
#define LWIP_RAW 1
#define TCP_MSS 1460
// SiguesAhi: un registro TLS de 16 KB no cabe en una ventana de 16 KB y la
// conexión se trabaría, ver pico-examples/pico_w/wifi/tls_client/lwipopts.h
#define TCP_WND 32768
#define TCP_SND_BUF (8 * TCP_MSS)
#define TCP_SND_QUEUELEN ((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define LWIP_NETIF_HOSTNAME 1
#define LWIP_NETCONN 0
#define MEM_STATS 0
#define SYS_STATS 0
#define MEMP_STATS 0
#define LINK_STATS 0
#define LWIP_CHKSUM_ALGORITHM 3
#define LWIP_DHCP 1
#define LWIP_IPV4 1
#define LWIP_TCP 1
#define LWIP_UDP 1
#define LWIP_DNS 1
#define LWIP_TCP_KEEPALIVE 1
#define LWIP_NETIF_TX_SINGLE_PBUF 1
#define DHCP_DOES_ARP_CHECK 0
#define LWIP_DHCP_DOES_ACD_CHECK 0

// SiguesAhi: TLS con mbedTLS
#define LWIP_ALTCP 1
#define LWIP_ALTCP_TLS 1
#define LWIP_ALTCP_TLS_MBEDTLS 1
// lwIP no exige un certificado válido: SiguesAhi revisa el resultado de la
// verificación después del saludo, así puede permitir conexiones inseguras
// para pruebas sin otra configuración
#define ALTCP_MBEDTLS_AUTHMODE MBEDTLS_SSL_VERIFY_OPTIONAL

// SiguesAhi: hora por SNTP, necesaria para revisar la vigencia de los
// certificados
#define SNTP_SERVER_DNS 1
#define SNTP_MAX_SERVERS 2
#define SNTP_STARTUP_DELAY 0
#ifdef __cplusplus
extern "C" {
#endif
void siguesahi_pico_fijar_hora(uint32_t segundos);
#ifdef __cplusplus
}
#endif
#define SNTP_SET_SYSTEM_TIME(segundos) siguesahi_pico_fijar_hora(segundos)

#ifndef NDEBUG
#define LWIP_DEBUG 1
#define LWIP_STATS 1
#define LWIP_STATS_DISPLAY 1
#endif

#endif
