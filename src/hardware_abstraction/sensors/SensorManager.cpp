/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/2 上午 11:25
 */

#include "hardware_abstraction/sensors/SensorManager.h"
#include "hardware_abstraction/sensors/LightSensor.h"
#include <Arduino.h>

SensorManager *SensorManager::instance = nullptr;

// 获取单例实例的方法
SensorManager *SensorManager::getInstance() {
    if (instance == nullptr) {
        instance = new SensorManager();
    }
    return instance;
}

// 私有构造函数
SensorManager::SensorManager() {
    // 初始化数据管理器实例
    dataManager = DataManager::getInstance();

    // 初始化温湿度传感器实例
    tempHumSensor = TemperatureHumiditySensor::getInstance(ProjectConfig::DHTPIN, ProjectConfig::DHTTYPE);

    // 初始化光照传感器实例
    lightSensor = LightSensor::getInstance();

    // 初始化电位器传感器实例
    potentiometerSensor = PotentiometerSensor::getInstance();
}

// 读取湿度
float SensorManager::readHumidity() {
    // 使用温湿度传感器实例读取湿度
    return tempHumSensor->readHumidity();
}

// 读取温度
float SensorManager::readTemperature() {
    // 使用温湿度传感器实例读取温度
    return tempHumSensor->readTemperature();
}

// 读取光照强度
int SensorManager::updateLightIntensity() {
    // 使用光照传感器实例读取光照强度
    return lightSensor->readLightIntensity();
}

// 读取电位器值
int SensorManager::updatePotentiometerValue() {
    // 使用电位器传感器实例读取电位器值
    return potentiometerSensor->readValue();
}


/**根据光照强度和可变电阻值推荐理想的灯光强度
 *
 * 推荐光线算法规则：
 * 如果衣柜关闭，即感受到的光照强度小于15%,那么认为衣柜没有打开门。衣柜不开启辅助灯光。如果可调电阻值为0，则不开启灯光
 * 衣柜开启后，根据可调电阻和光照情况调整光线。根据可调电阻的权值和光照情况的权值6:4来调整。
 * 也就是说，光照强度越小，灯光越明亮；可调电阻值越大，灯光越明亮；用户能够调整的范围为60%。
 *
 * @param lightIntensity 现在的光照强度
 * @param potValue 用户输入的可变电阻值
 */
int SensorManager::recommendLightIntensity(int lightIntensity, int potValue) {
    // 记录调试数据
    dataManager->logData("light: intensity" + String(lightIntensity), false);
    dataManager->logData("potvalue" + String(potValue), false);

    // 灯光的建议调整值
    int light = 0;

    // 如果可变电阻值为0(很小的一个值）,或者本来衣柜就没有开门，那么不开启灯光
    if (!(potValue < 5 || lightIntensity < 15)) {
        // 衣柜开启时，默认灯光强度75%,根据可调电阻和光照情况调整光线
        light = 75;

        // 光照强度的权重为40%
        int sensorWeight = (100 - lightIntensity) * 0.4;
        // 可调电阻的权重为60%
        int potWeight = potValue * 0.6;

        // 综合考虑光照和可调电阻，选最大值，得到充分的照明
        light = std::max(light, sensorWeight + potWeight);

        // 记录调试数据
        dataManager->logData("user condition calculated light: " + String(sensorWeight + potWeight), false);
    }

    return light;
}




