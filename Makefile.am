SUBDIRS = src tests
ACLOCAL_AMFLAGS=-I m4

.PHONY: docker rpm

docker:
	podman build -f hack/Dockerfile.alpine -t dns-dev
rpm:
	rpmbuild -ba dns.spec