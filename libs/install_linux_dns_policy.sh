#!/bin/bash
# Run as the desktop user; pkexec requests authorization for installation only.
set -euo pipefail

if [[ $# -gt 1 || ( $# -eq 1 && $1 != --remove ) ]]; then
    echo "Usage: bash $0 [--remove]" >&2
    exit 2
fi
if [[ $(id -u) == 0 ]]; then
    echo "Run this script as your normal desktop user, without sudo." >&2
    exit 1
fi

policy_user=$(id -un)
policy_uid=$(id -u)
# Restrict interpolation into JavaScript to ordinary account-name characters.
if [[ ! $policy_user =~ ^[a-zA-Z_][a-zA-Z0-9_.-]*\$?$ ]]; then
    echo "Unsupported account name: $policy_user" >&2
    exit 1
fi
policy_path="/etc/polkit-1/rules.d/49-nekobox-resolved-$policy_uid.rules"
if [[ ${1:-} == --remove ]]; then
    pkexec /usr/bin/rm -f -- "$policy_path"
    echo "Removed $policy_path"
    exit 0
fi

policy_temp=$(mktemp -d)
trap 'rm -rf -- "$policy_temp"' EXIT
cat > "$policy_temp/nekobox.rules" <<EOF
// NekoBox DNS setup for one active local user and the neko-tun interface.
// Other applications belonging to this user may also configure DNS on neko-tun.
// Missing interface details deliberately fall back to the system policy.
polkit.addRule(function(action, subject) {
    if (subject.user !== "$policy_user" || !subject.local || !subject.active ||
        action.lookup("interface") !== "neko-tun") {
        return;
    }
    if (action.id === "org.freedesktop.resolve1.set-dns-servers" ||
        action.id === "org.freedesktop.resolve1.set-domains" ||
        action.id === "org.freedesktop.resolve1.set-default-route") {
        return polkit.Result.YES;
    }
});
EOF

echo "Installing DNS authorization for $policy_user on neko-tun only."
pkexec /usr/bin/install -o root -g root -m 0644 -- "$policy_temp/nekobox.rules" "$policy_path"
echo "Installed $policy_path. Restart your NekoBox profile to check activation."
