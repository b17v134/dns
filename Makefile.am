SUBDIRS = src tests python
ACLOCAL_AMFLAGS=-I m4

.PHONY: docker rpm

docker:
	podman build -f hack/Dockerfile.alpine -t dns-dev-alpine
	podman build -f hack/Dockerfile.fedora -t dns-dev-fedora
	podman build -f hack/Dockerfile.ubuntu -t dns-dev-ubuntu
rpm:
	rpmbuild -ba dns.spec