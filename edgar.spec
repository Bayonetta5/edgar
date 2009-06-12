Summary: 2D Platform Game
Name: %{name}
Version: %{version}
Release: %{release}
Source: %{name}-%{version}-%{release}.tar.gz
Vendor: Parallel Realities
Packager: Richard Sweeney
URL: http://www.parallelrealities.co.uk/edgar.php
Group: Games/Arcade
License: GPL
%description
The Legend of Edgar. When his father fails to return home after venturing
out one stormy night, Edgar sets off on a quest to rescue him.

%prep
%setup -q

%build
make VERSION=%{version} RELEASE=%{release}

%install
make install

%clean

%post


%files
%dir /usr/games
%dir /usr/share/games/edgar
%dir /usr/share/doc/%{name}
%dir /usr/share/applnk
/usr/games/%{name}
/usr/share/games/edgar/%{name}.pak
/usr/share/doc/%{name}/*
/usr/share/applications/edgar.desktop
/usr/share/icons/hicolor/16x16/apps/edgar.png
/usr/share/icons/hicolor/32x32/apps/edgar.png
/usr/share/icons/hicolor/64x64/apps/edgar.png