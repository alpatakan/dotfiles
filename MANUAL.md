# Manual Steps

Things that `chezmoi apply` can't (or shouldn't) do unattended. Work through this list after the first `chezmoi init --apply`.

---

## 1. Add the SSH public key to GitHub

`run_once_after_04-ssh-key.sh.tmpl` generates `~/.ssh/id_ed25519` and prints the public key. Upload it:

```bash
# if gh is already authenticated
gh auth login        # device flow, one-time
gh ssh-key add ~/.ssh/id_ed25519.pub --title "$(hostname)"
```

Or paste into <https://github.com/settings/ssh/new>.

Then switch the dotfiles remote to SSH so future `chezmoi update`s use the key:

```bash
chezmoi cd
git remote set-url origin git@github.com:alpatakan/dotfiles.git
exit
```

## 2. Work git identity (automated — only manual if you skipped it)

If you provided `work_email` at init, chezmoi already generated `~/.gitconfig-work` and `~/.gitconfig` already has an `includeIf` for your work folder (`~/axon` by default). Nothing to do.

If you skipped (left `work_email` empty at init) and want to add it later:

```bash
chezmoi state delete-bucket --bucket=entryState   # lets init re-prompt
chezmoi init                                       # re-run prompts
chezmoi apply -v
```

The default git identity applies everywhere **except** repos under your configured work folder — those use the work identity automatically via `includeIf`.

## 3. Reboot

Auto-login and the GDM dconf lock-screen changes only take effect on reboot.

```bash
sudo systemctl reboot
```

## 4. GNOME Remote Desktop — TLS cert + credentials

`gnome-remote-desktop` ships RDP with **mandatory TLS**, but you still need to supply the cert and set a username/password. One-time:

```bash
mkdir -p ~/.local/share/gnome-remote-desktop

openssl req -new -newkey rsa:4096 -days 3650 -nodes -x509 \
    -subj "/CN=$(hostname)" \
    -out ~/.local/share/gnome-remote-desktop/rdp-tls.crt \
    -keyout ~/.local/share/gnome-remote-desktop/rdp-tls.key

grdctl rdp set-tls-cert ~/.local/share/gnome-remote-desktop/rdp-tls.crt
grdctl rdp set-tls-key  ~/.local/share/gnome-remote-desktop/rdp-tls.key
grdctl rdp set-credentials '<rdp-username>' '<rdp-password>'   # NOT your login password
grdctl rdp enable

systemctl --user enable --now gnome-remote-desktop.service
```

Verify:

```bash
grdctl status
ss -tlnp | grep 3389
```

## 5. Remote-desktop over the internet — SSH tunnel, do **not** forward 3389

RDP-over-WAN is a bad target. Tunnel it over SSH on the already-hardened port 2211:

**On the router:** forward **only** TCP `2211` → work PC.

**From home:**

```bash
# start the tunnel
ssh -p 2211 -L 3389:localhost:3389 -N alp@<work-public-ip>

# in a separate terminal / RDP client, connect to:
# localhost:3389
```

Any RDP client (Remmina, mstsc, FreeRDP) works against `localhost:3389`. The SSH connection provides both the tunnel and the authentication (key-only).

## 6. Flathub apps

`chezmoi` enables Flathub but installs nothing from it. Pick what you want:

```bash
flatpak install -y flathub com.slack.Slack
flatpak install -y flathub com.discordapp.Discord
flatpak install -y flathub org.signal.Signal
flatpak install -y flathub com.spotify.Client
flatpak install -y flathub org.telegram.desktop
```

## 7. VSCode extensions

Export the list from your existing machine once, then re-import here:

```bash
# on the old box
code --list-extensions > ~/vscode-extensions.txt

# on this box (after copying the list)
xargs -L1 code --install-extension < ~/vscode-extensions.txt
```

VSCode settings/keybindings sync via the built-in Settings Sync (GitHub login) — simpler than tracking JSON files.

## 8. Log out / log back in for docker group

If `install_docker = true`, your user was added to the `docker` group. The membership only takes effect on the **next login**, not with `newgrp` or the chezmoi run. Log out and back in (or reboot — which you're doing anyway for step 3).

Verify:

```bash
id -nG | tr ' ' '\n' | grep docker
docker run --rm hello-world
```

## 9. Firefox / Chrome / browser sign-in

Obvious, listing it so the checklist is complete.

## 10. GNOME Extensions (optional)

If you used extensions on the previous box, note them here for future re-installs:

- (list any you end up installing: blur-my-shell, dash-to-dock, etc.)

Install from <https://extensions.gnome.org> or via `gnome-extensions` CLI.

## 11. Work-specific VPN / proxy / tools

Whatever your employer requires — list them here so future-you doesn't forget:

- (VPN client)
- (internal CA certs)
- (company-specific CLI tools)
