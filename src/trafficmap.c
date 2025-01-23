/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2025 Daniel Morante
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <err.h>

#define MAX_LINE_LENGTH 256
#define MAX_REMAP_LINE_LENGTH 256

// Function to get SRV record for a domain
int get_srv_record(const char *domain, char *port, char *target) {
    unsigned char answer[NS_PACKETSZ];
    int len; 

    // initialize the resolver
    res_init(); 
    
    len = res_search(domain, C_IN, T_SRV, answer, sizeof(answer));
    if (len == -1) {
        warn("res_search => domain: %s, size: %d", domain, len);
        return -1;
    }

    ns_msg msg;
    ns_initparse(answer, len, &msg);
    int cnt = ns_msg_count(msg, ns_s_an);
    if (cnt < 1) {
        warnx("No SRV records found for %s", domain);
        return -1;
    }

    for (int i = 0; i < cnt; i++) {
        ns_rr rr;
        ns_parserr(&msg, ns_s_an, i, &rr);
        if (ns_rr_type(rr) == T_SRV) {
            const unsigned char *rdata = ns_rr_rdata(rr);

            u_int16_t port_num = ns_get16(rdata + 2 * NS_INT16SZ);
            sprintf(port, "%d", port_num);

            len = dn_expand(ns_msg_base(msg), ns_msg_end(msg), rdata + 3 * NS_INT16SZ, target, NS_MAXDNAME);
            //len = ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg), rdata + 3 * NS_INT16SZ, target, NS_MAXDNAME);
            if (len == -1) {
                warn("Unable to lookup target for %s", domain);
                return -1;
            }

            return 0;
        }
    }

    return -1;
}

int main(int argc, char *argv[]) {
    if (argc!= 3) {
        errx(1, "Usage: %s <input_file> <output_file>", argv[0]);
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        err(1, "fopen %s", argv[1]);
    }

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        err(1, "fopen %s", argv[2]);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, input_file)!= NULL) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }

        // Construct the full domain name for the SRV record
        char full_domain[NS_MAXDNAME];
        sprintf(full_domain, "_http._tcp.%s", line);

        // Get SRV record for domain
        char port[MAX_LINE_LENGTH];
        char location[NS_MAXDNAME];
        if (get_srv_record(full_domain, port, location) == -1) {
            continue;
        }

        // Generate remap configuration line
        char remap_line[MAX_REMAP_LINE_LENGTH];
        sprintf(remap_line, "map http://%s http://%s:%s\n", line, location, port);
        fprintf(output_file, "%s", remap_line);
    }

    fclose(input_file);
    fclose(output_file);
    return 0;
}