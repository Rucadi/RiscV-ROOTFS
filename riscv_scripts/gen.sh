export RUNLEVEL=1
export DEBIAN_FRONTEND=noninteractive


dpkg --configure -a
dpkg --configure -a
apt clean
rm -rf /var/lib/apt/lists/*

rm -rf  /usr/share/doc/* 
rm -rf /usr/share/groff/* /usr/share/info/*
rm -rf /usr/share/lintian/* /usr/share/linda/* /var/cache/man/*
rm -rf /usr/share/man/*

