FROM fedora:29

LABEL description="Builds Shhh! server code"

RUN dnf update -y
RUN dnf install @development-tools clang openssl-devel -y

WORKDIR /code
