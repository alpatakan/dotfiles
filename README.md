# dotfiles

Configs + bootstrap scripts for a Fedora workstation, managed by [chezmoi](https://www.chezmoi.io).

A fresh Fedora install should become a working dev box with **two commands**.

## Quickstart (fresh machine)

```bash
sudo dnf install -y chezmoi
chezmoi init --apply alpatakan
```

On first run, chezmoi prompts for:
- `default_email` / `default_name` — git identity used for everything except work repos
- `work_email` / `work_name` — git identity for repos under the work folder (leave empty to skip)
- `work_folder` — where work repos live (default `~/axon`); `includeIf` in `.gitconfig` picks the work identity automatically when inside it
- `install_docker` — install docker-ce alongside podman
- `accent_color` — per-machine accent (cyan/green/yellow/blue/magenta/red/white or a 256-color index); drives the zsh prompt color + tmux active-window / pane-border color so you don't confuse boxes

It then runs the `run_once_before_*` scripts (enable repos, install packages, configure sshd + GNOME), writes all dotfiles to `$HOME`, and runs the `run_once_after_*` scripts (oh-my-zsh, fnm + Node LTS, vim-plug, ed25519 SSH key).

When it finishes, follow the steps in [MANUAL.md](./MANUAL.md) (upload the SSH key to GitHub, create `~/.gitconfig-work`, set up GNOME Remote Desktop TLS, etc.) and reboot.

## What gets installed

| Category | Packages / tools |
|---|---|
| Toolchain | `development-tools`, `c-development` (dnf groups) |
| Shell | `zsh`, `oh-my-zsh`, `pure` (sindresorhus/pure), `zsh-autosuggestions`, `zsh-syntax-highlighting` |
| Editor | `vim` with vim-plug → fugitive, gruvbox, tagbar, fzf.vim, ALE, vim-javascript (plus `neovim` available for future use) |
| Terminal | `ghostty` (via `scottames/ghostty` COPR) |
| CLI | `tmux`, `fzf`, `ripgrep`, `btop`, `htop`, `fd-find`, `bat`, `zoxide`, `gh`, `jq`, `diff-so-fancy`, `tree`, `ShellCheck` |
| Node | `fnm` + Node LTS |
| Containers | Optional: `docker-ce` + compose plugin (podman is always present on Fedora) |
| Remote access | `gnome-remote-desktop` (RDP with mandatory TLS) |
| System | sshd (default port 22, key + password auth, no root), firewalld ssh service enabled, GNOME no-sleep, GDM auto-login |

## Repo layout

```
dotfiles/
├── README.md              # You are here
├── MANUAL.md              # One-time manual steps (GitHub key upload, TLS cert, etc.)
├── LICENSE                # MIT
├── .chezmoiroot           # Tells chezmoi the source lives in home/
└── home/                  # chezmoi source tree (everything below here is managed)
    ├── .chezmoi.toml.tmpl         # First-run prompts
    ├── .chezmoiignore
    ├── dot_tmux.conf.tmpl         → ~/.tmux.conf (accent color + C-a leader)
    ├── dot_vimrc                  → ~/.vimrc
    ├── dot_zshrc.tmpl             → ~/.zshrc (loads Pure + machine accent)
    ├── dot_gitconfig.tmpl         → ~/.gitconfig (default identity + includeIf for work)
    ├── dot_gitconfig-work.tmpl    → ~/.gitconfig-work (only if work_email set)
    ├── dot_config/ghostty/config  → ~/.config/ghostty/config
    ├── private_dot_ssh/config.tmpl → ~/.ssh/config (0600)
    ├── run_once_before_01-enable-repos.sh.tmpl
    ├── run_once_before_02-install-packages.sh.tmpl
    ├── run_once_before_03-system-config.sh.tmpl
    ├── run_once_after_01-oh-my-zsh.sh.tmpl
    ├── run_once_after_02-fnm-node.sh.tmpl
    ├── run_once_after_03-vim-plug.sh.tmpl
    └── run_once_after_04-ssh-key.sh.tmpl
```

### chezmoi filename conventions

- `dot_X` → `~/.X`
- `private_X` → `chmod 0600` (or 0700 for dirs)
- `X.tmpl` → rendered through Go `text/template` using `.chezmoi.toml` data
- `run_once_before_*` / `run_once_after_*` → scripts executed on first apply (tracked by hash in chezmoi state)

## Common workflows

```bash
# See what would change
chezmoi diff

# Apply changes
chezmoi apply -v

# Edit a file in the source tree (opens $EDITOR)
chezmoi edit ~/.zshrc

# Pull upstream changes and apply
chezmoi update

# Re-run a run_once script (e.g. after editing it)
chezmoi state delete-bucket --bucket=scriptState
chezmoi apply

# Jump to the source repo
chezmoi cd
```

### Using an existing clone as the source

By default chezmoi clones to `~/.local/share/chezmoi`. To point it at a clone you already have (e.g. `~/alpatakan/dotfiles`):

```bash
mkdir -p ~/.config/chezmoi
cat > ~/.config/chezmoi/chezmoi.toml <<EOF
sourceDir = "$HOME/alpatakan/dotfiles"
EOF
chezmoi apply -v
```

## License

MIT — see [LICENSE](./LICENSE).
