#
# Copyright(c) 2025 Intel Corporation
# SPDX-License-Identifier: BSD-3-Clause
#

.PHONY: default all
default: all

all: deb_sgx_pcs_client_pkg rpm_sgx_pcs_client_pkg

.PHONY: deb_sgx_pcs_client_pkg
deb_sgx_pcs_client_pkg:
	./installer/linux/deb/intel-tee-pcs-client-tool/build.sh

.PHONY: rpm_sgx_pcs_client_pkg
rpm_sgx_pcs_client_pkg:
	./installer/linux/rpm/intel-tee-pcs-client-tool/build.sh

clean:
	./installer/linux/deb/intel-tee-pcs-client-tool/clean.sh
	./installer/linux/rpm/intel-tee-pcs-client-tool/clean.sh

test:
	python3 -m venv .venv && . ./.venv/bin/activate && pip install -r requirements.txt && python -m unittest discover && deactivate
