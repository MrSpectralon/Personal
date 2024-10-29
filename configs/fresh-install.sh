# Currenntly not tested the install sequence - so probably don't work as intended.
sudo apt install neovim -y
sudo apt install git -y
sudo apt install gnome-shell-extensions -y
sudo apt install zsh -y
sudo apt install curl -y
sudo apt install build-essential -y
sudo apt install fzf -y
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
git clone git@github.com:MrSpectralon/Personal.git ~/Documents/
cat ~/Documents/Personal/configs/.zshrc > ~/.zshrc
git clone https://github.com/zsh-users/zsh-history-substring-search ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-history-substring-search
git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
cargo install zoxide --locked
zoxide init --cmd cd zsh
