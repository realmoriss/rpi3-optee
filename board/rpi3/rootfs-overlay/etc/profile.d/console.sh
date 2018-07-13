#!/bin/sh

alias ls='ls -h'
alias ll='ls -l'
alias la='ls -al'

export EDITOR='/bin/nano'
export VISUAL='/bin/nano'
export PS1='$(whoami)@$(hostname):$(pwd)$ '
