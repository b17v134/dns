SUBDIRS = src tests python
ACLOCAL_AMFLAGS=-I m4

.PHONY: autoformat docker format rpm

autoformat:
	find . -iname *.h -o -iname *.c | xargs clang-format --style=WebKit -i

docker:
	podman build -f hack/Dockerfile.alpine -t dns-dev-alpine
	podman build -f hack/Dockerfile.fedora -t dns-dev-fedora
	podman build -f hack/Dockerfile.ubuntu -t dns-dev-ubuntu

format:
	find . -iname *.h -o -iname *.c | xargs clang-format --style=WebKit --dry-run --Werror

rpm:
	rpmbuild -ba dns.spec