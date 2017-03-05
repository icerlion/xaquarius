The boost should at the place of "\boost_1_*_0" avoid too big size in svn.
Compile option for 64 bit:
b2 address-model=64

////////////////////////////////////////////////////////////////////////////
Install boost on CentOS
wget http://sourceforge.net/projects/boost/files/boost/1.*/boost_1_*.zip
unzip boost_1_*.zip 
cd boost_1_* 
./bootstrap.sh 
./b2 address-model=64

////////////////////////////////////////////////////////////////////////////
// Depends lib list:
yum -y install bzip2-devel.x86_64
yum -y install zlib-devel.x86_64
yum -y install python-devel.x86_64