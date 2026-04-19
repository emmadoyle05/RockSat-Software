# WALLOPS: WIDE ANGLE LIDAR LASER OPERATING IN PRESSURIZED SPACE
## LIDAR POINT CLOUD COLLECTION 2-AXIS GIMBAL ON COLLEGE OF THE CANYONS' ATMOLIS NASA ROCKSAT PAYLOAD
### Officially launching in a sounding rocket June 2026

### Setting up on new Pi

(NOTE: Requires internet access to download lgpio and dependencies.)

```
sudo make setup
sudo make run
```

### Cleaning and running when code changes

(NOTE: Will delete data in scan_data, scan_logs, and lidar. If there's something in here you want, please move before running clean.)

```
sudo make clean
sudo make run
```