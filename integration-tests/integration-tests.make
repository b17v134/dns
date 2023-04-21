.PHONY: integration-tests-docker integration-tests-bind-start integration-tests-bind-stop

integration-tests-docker:
	cd $(PWD)/integration-tests && sudo podman build -f ./docker/Dockerfile -t bind

integration-tests-bind-start:
	sudo podman network create --subnet 10.23.0.0/24 bind
	cd $(PWD)/integration-tests && sudo podman run \
		-d \
		--network bind \
		--ip 10.23.0.7 \
		-v $(PWD)/integration-tests/testdata/bind/named.conf.local:/etc/bind/named.conf.local:Z \
		-v $(PWD)/integration-tests/testdata/bind/named.conf.options:/etc/bind/named.conf.options:Z \
		-v $(PWD)/integration-tests/testdata/bind/example.test-bind:/etc/bind/example.test-bind:Z \
		--name bind \
		bind

integration-tests-bind-stop:
	sudo podman rm -f bind || true
	sudo podman network rm bind

.PHONY: integration-tests-build-dns
integration-tests-build-dns:
	rm -rf integration-tests-dns
	./configure --prefix=$(PWD)/integration-tests-dns
	$(MAKE) clean check install

.PHONY: integration-tests
integration-tests: 
	$(MAKE) integration-tests-bind-start
	ruby integration-tests/tests.rb -v
	$(MAKE) integration-tests-bind-stop