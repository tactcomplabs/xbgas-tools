#!/bin/sh
exec "`dirname "$0"`/configure" --disable-install-jim --with-ext="eventloop array clock regexp stdlib tclcompat" --without-ext="default" "$@"
