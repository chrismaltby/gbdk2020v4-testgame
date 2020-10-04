# GBDK2020v4 Test Game for GB Studio

Test project to determine changes needed to GB Studio engine to support GBDK2020v4

Use gbdk2020v4 releases from here https://github.com/Zal0/gbdk-2020/releases/tag/v4.0

```
export GBDKDIR="/Users/cmaltby/Desktop/gbdk2020/"
export PATH="/Users/cmaltby/Desktop/gbdk2020/bin:$PATH"
export CART_SIZE=16
make clean
make rom
```

ROM is created as `build/rom/game.gb`
