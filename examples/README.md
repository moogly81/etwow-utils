# Examples

These examples purose is to validate invidual functionalities, such as BLE advertising, BLE scanning, uart listen, uart pass-through, etc ...



## Configure 

```
brew install arduino-cli
```

Configure the sketch.yaml files in the folder

Find the right PORT and FQBN:
   - plug in your micro-controller  
   - run `arduino-cli board list` 
   - configure each folder's sketch.yaml

```
arduino-cli core update-index
arduino-cli core install esp32:esp32
```

## compile, send and monitor

```
cd <dir>
arduino-cli compile
arduino-cli upload
arduino-cli monitor
```


## upgrade components 
Update core 

```
arduino-cli core update-index --additional-urls ...
````

Update libraries 

``` 
arduino-cli lib list --updatable --all --additional-urls https://static-cdn.m5stack.com/resource/arduino/package_m5stack_index.json
```
