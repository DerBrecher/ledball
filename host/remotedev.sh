#!/bin/bash

rm testlocal.py

sshpass -p "1337" scp robin@vmdev:/home/robin/Desktop/remotedev/host/imagetest/displayeffect.py testlocal.py

python testlocal.py $1 $2
