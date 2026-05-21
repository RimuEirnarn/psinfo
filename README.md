# Rimu's PowerShell Startup Information

It prints system informations it can try to grab. Why C? Simply because PowerShell is slow.

## Building

You just need some windows stuff around, and then gcc. Basic stuff, I forgot what did I use, simply check Makefile for more information.

I suggest you to have an abomination of UNIX-style commands and Windows ones, unless you have working gcc, rm, cp around without msys or similiar AND, also, without PowerShell's atrocious aliases.

How to build? Really?

```bash
make && make install
```

If you want to install it to other places, set `$PREFIX` (or `$RIMU_PREFIX`) as env before executing `make install`

