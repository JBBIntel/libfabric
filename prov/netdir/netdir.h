/*
* Copyright (c) 2015-2016 Intel Corporation, Inc.  All rights reserved.
*
* This software is available to you under a choice of one of two
* licenses.  You may choose to be licensed under the terms of the GNU
* General Public License (GPL) Version 2, available from the file
* COPYING in the main directory of this source tree, or the
* BSD license below:
*
*     Redistribution and use in source and binary forms, with or
*     without modification, are permitted provided that the following
*     conditions are met:
*
*      - Redistributions of source code must retain the above
*        copyright notice, this list of conditions and the following
*        disclaimer.
*
*      - Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following
*        disclaimer in the documentation and/or other materials
*        provided with the distribution.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef _FI_NETDIR_H_
#define _FI_NETDIR_H_

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <ws2tcpip.h>
#include <winsock2.h>
#include <assert.h>

#include <ndspi.h>
#include "fi_osd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OFI_ND_MAJOR_VERSION 1
#define OFI_ND_MINOR_VERSION 5

#define ND_MSG_IOV_LIMIT (256)
#define ND_MSG_INTERNAL_IOV_LIMIT (512)

#define ND_EP_MAX_CM_DATA_SIZE (256)

#define OFI_ND_PRIMARY_CAPS					\
	FI_MSG | FI_RMA | FI_SEND | FI_RECV |			\
	FI_READ | FI_WRITE | FI_REMOTE_READ | FI_REMOTE_WRITE
#define OFI_ND_CAPS OFI_ND_PRIMARY_CAPS

extern struct gl_data {
	int	inline_thr;
	int	prepost_cnt;
	int	prepost_buf_cnt;
	int	flow_control_cnt;
} gl_data;

extern struct fi_provider ofi_nd_prov;
extern struct util_prov ofi_nd_util_prov;
extern struct fi_info ofi_nd_info;

extern const char ofi_nd_prov_name[];

int ofi_nd_cq_open(struct fid_domain *domain, struct fi_cq_attr *attr,
		   struct fid_cq **cq_fid, void *context);
int ofi_nd_eq_open(struct fid_fabric *fabric, struct fi_eq_attr *attr,
		   struct fid_eq **peq, void *context);
int ofi_nd_endpoint(struct fid_domain *domain, struct fi_info *info,
		    struct fid_ep **ep_fid, void *context);
int ofi_nd_passive_endpoint(struct fid_fabric *fabric, struct fi_info *info,
			    struct fid_pep **pep, void *context);
int ofi_nd_domain_open(struct fid_fabric *fabric, struct fi_info *info,
		       struct fid_domain **domain, void *context);
int ofi_nd_srx_ctx(struct fid_domain *domain,
		   struct fi_rx_attr *attr, struct fid_ep **rx_ep,
		   void *context);
int ofi_nd_fabric(struct fi_fabric_attr *attr, struct fid_fabric **fabric,
		  void *context);

int ofi_nd_getinfo(uint32_t version, const char *node, const char *service,
		   uint64_t flags, struct fi_info *hints, struct fi_info **info);
void ofi_nd_fini(void);

int ofi_nd_mr_reg(struct fid *fid, const void *buf, size_t len,
		  uint64_t access, uint64_t offset, uint64_t requested_key,
		  uint64_t flags, struct fid_mr **mr, void *context);
int ofi_nd_mr_regv(struct fid *fid, const struct iovec *iov,
		   size_t count, uint64_t access,
		   uint64_t offset, uint64_t requested_key,
		   uint64_t flags, struct fid_mr **mr, void *context);
int ofi_nd_mr_regattr(struct fid *fid, const struct fi_mr_attr *attr,
		      uint64_t flags, struct fid_mr **mr);
int ofi_nd_cntr_open(struct fid_domain *pdomain, struct fi_cntr_attr *attr,
		     struct fid_cntr **pcntr, void *context);

typedef int(*ofi_nd_adapter_cb_t)(const ND2_ADAPTER_INFO* info, const char *name);

void ofi_nd_send_event(ND2_RESULT *result);
void ofi_nd_read_event(ND2_RESULT *result);
void ofi_nd_write_event(ND2_RESULT *result);

HRESULT ofi_nd_startup(ofi_nd_adapter_cb_t cb);
HRESULT ofi_nd_shutdown();

int ofi_nd_lookup_adapter(const char *name, IND2Adapter **adapter, struct sockaddr** addr);

int ofi_nd_is_valid_addr(const SOCKADDR *addr);
int ofi_nd_addr_cmp(const void* vaddr1, const void* vaddr2);

int ofi_nd_is_same_file(const wchar_t* path1, const wchar_t* path2);
int ofi_nd_file_exists(const wchar_t* path);
int ofi_nd_is_directory(const wchar_t* path);
const wchar_t *ofi_nd_filename(const wchar_t *path);

static inline size_t unique(void *base, size_t num, size_t width,
			    int(*cmp)(const void *, const void *))
{
	char *dst = (char*)base;
	char *src = (char*)base + width;

	size_t i;
	size_t n = 1;

	if (num < 2)
		return num;

	for (i = 1; i < num; i++) {
		if (cmp(dst, src)) {
			dst += width;
			if (dst != src)
				memcpy(dst, src, width);
			n++;
		}
		src += width;
	}

	return n;
}

#define OFI_ND_TIMEOUT_INIT(timeout)				\
	uint64_t sfinish = ((timeout) >= 0) ?			\
		(fi_gettime_ms() + (timeout) * 10000) : -1;

#define OFI_ND_TIMEDOUT() ((sfinish > 0) ? fi_gettime_ms() >= sfinish : 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FI_NETDIR_H_ */

