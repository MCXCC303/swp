> swp Usage:

## 1. Set default swap target

```
./swp
```

This will gives a brief setup and config file will be saved in `$HOME/.swpcnf` so that next time you don't have to type them again.

## 2. Add swap files

```
./swp file1 file2 ...
```

Files will be copied into `$HOME/swap` waiting to sync.

## 3. Sync

Use `./swp` again after file select done and there will gives 3 options:

```
Using config to sync:

        ip: 11.4.51.4
        username: sodayo
        port: 19810

[ L (pull) | S (push) | E (edit) ]
```

Choose pull/push to sync between target like git, choose edit to modify config

- Note: push will first clean swaps in opponent system, and pull will first clean swaps in local; This aims to make serious sync between systems, however I have not come up with how to make this more reasonable but to use, just remember push first

- Note: move the compiled binary file "swp" to `/usr/local/bin/` to use it anywhere
