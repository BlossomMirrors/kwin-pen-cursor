#!/bin/bash
set -e

NAME=kwin-pen-cursor
CURRENT_VERSION=$(grep -o '"Version": "[^"]*' src/pencursor.json | cut -d'"' -f4)

read -rp "Version [$CURRENT_VERSION]: " VERSION
VERSION=${VERSION:-$CURRENT_VERSION}

read -rp "Release [1]: " RELEASE
RELEASE=${RELEASE:-1}

read -rp "Changelog entry [packaged $NAME $VERSION]: " CHANGELOG
CHANGELOG=${CHANGELOG:-"packaged $NAME $VERSION"}

if [ "$VERSION" != "$CURRENT_VERSION" ]; then
    sed -i "s/\"Version\": \"$CURRENT_VERSION\"/\"Version\": \"$VERSION\"/" src/pencursor.json
    echo "Updated version in pencursor.json to $VERSION"
fi

RPMBUILD=~/rpmbuild
mkdir -p "$RPMBUILD"/{SPECS,SOURCES,BUILD,RPMS,SRPMS} release

tar -czf "$RPMBUILD/SOURCES/$NAME-$VERSION.tar.gz" \
    --transform "s|^\./|$NAME-$VERSION/|" \
    --exclude=./.git --exclude=./release --exclude=./build \
    .

cat > "$RPMBUILD/SPECS/$NAME.spec" << EOF
Name:           $NAME
Version:        $VERSION
Release:        $RELEASE%{?dist}
Summary:        KWin effect that shows a cursor overlay when using a pen tablet
License:        MIT
URL:            https://github.com/vaporvee/kwin-pen-cursor
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  kf6-kcoreaddons-devel
BuildRequires:  kf6-kconfig-devel
BuildRequires:  kf6-kconfigwidgets-devel
BuildRequires:  kf6-kcmutils-devel
BuildRequires:  kf6-kio-devel
BuildRequires:  kwin-devel
BuildRequires:  libxcb-devel

Requires:       kwin

%description
A KWin effect that shows a custom SVG cursor overlay when a pen tablet is
in proximity, hiding the system cursor in the process.

%prep
%autosetup

%build
%cmake -DPLUGIN_INSTALL_DIR=%{_libdir}/qt6/plugins
%cmake_build

%install
%cmake_install

%files
%{_libdir}/qt6/plugins/kwin/effects/plugins/libkwin_pencursor.so
%{_libdir}/qt6/plugins/kwin/effects/configs/kwin_pencursor_config.so
%{_datadir}/kwin/effects/pencursor.json
%{_datadir}/kwin/effects/pen-point-cursor.svg

%changelog
* $(LC_TIME=C date "+%a %b %d %Y") packager - $VERSION-$RELEASE
- $CHANGELOG
EOF

rpmbuild -ba "$RPMBUILD/SPECS/$NAME.spec"

find "$RPMBUILD/RPMS" -name "$NAME-$VERSION-$RELEASE*.rpm" -exec cp {} release/ \;
echo "Done: $(ls release/$NAME-$VERSION-$RELEASE*.rpm 2>/dev/null)"
