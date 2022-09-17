package com.example.greenhouse;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.RecyclerView;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.os.Handler;
import android.os.Message;
import android.media.Image;
import android.provider.Settings;
import android.widget.ImageView;
import android.widget.TextView;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import org.json.JSONException;
import org.json.JSONObject;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import java.lang.reflect.GenericArrayType;

public class MainActivity extends AppCompatActivity {

    private Button Button_1 ;
    private Button Button_2 ;
    private String host = "tcp://iot.ranye-iot.net";
    private String userName = "GreenHouse_IoT_3";
    private String passWord = "Dk200010";
    private String mqtt_id = "GreenHouse_IoT_3_id";
    private String mqtt_sub_topic = "GreenHouse_IoT/Env_zt";
    private String mqtt_pub_topic = "GreenHouse_IoT/Env_kz";
    private MqttClient client;
    private MqttConnectOptions options;
    private Handler handler;
    private ScheduledExecutorService scheduler;
    private TextView Text_1;


    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button_1 = findViewById(R.id.Button_1);
        Button_1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(MainActivity.this," 获取数据中，请稍等...",Toast.LENGTH_SHORT).show();
                publishmessageplus(mqtt_pub_topic,"1");
            }
        });

        Button_2 = findViewById(R.id.Button_2) ;
        Button_2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Text_1.setText("数据已清空！请重新获取！");
            }
        });

        Text_1 = findViewById(R.id.Text_1) ;

        Mqtt_init();
        startReconnect();

        handler = new Handler(Looper.myLooper()) {
            @SuppressLint({"SetTextI18n", "HandlerLeak"})
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what){
                    case 3:  //MQTT 收到消息回传
                        Text_1.setText(msg.obj.toString());
                        break;
                    case 30:  //连接失败
                        Toast.makeText(MainActivity.this,"很遗憾！连接失败！" ,Toast.LENGTH_SHORT).show();
                        break;
                    case 31:   //连接成功
                        Toast.makeText(MainActivity.this,"恭喜！连接成功啦！" ,Toast.LENGTH_SHORT).show();
                        try {
                            client.subscribe(mqtt_sub_topic,1);
                        } catch (MqttException e) {
                            e.printStackTrace();
                        }
                        break;
                    default:
                        break;
                }
            }
        };
    }

    private void Mqtt_init()
    {
        try {
            //host为主机名，test为clientid即连接MQTT的客户端ID，一般以客户端唯一标识符表示，MemoryPersistence设置clientid的保存形式，默认为以内存保存
            client = new MqttClient(host, mqtt_id,
                    new MemoryPersistence());
            //MQTT的连接设置
            options = new MqttConnectOptions();
            //设置是否清空session,这里如果设置为false表示服务器会保留客户端的连接记录，这里设置为true表示每次连接到服务器都以新的身份连接
            options.setCleanSession(false);
            //设置连接的用户名
            options.setUserName(userName);
            //设置连接的密码
            options.setPassword(passWord.toCharArray());
            // 设置超时时间 单位为秒
            options.setConnectionTimeout(10);
            // 设置会话心跳时间 单位为秒 服务器会每隔1.5*20秒的时间向客户端发送个消息判断客户端是否在线，但这个方法并没有重连的机制
            options.setKeepAliveInterval(20);
            //设置回调
            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    //连接丢失后，一般在这里面进行重连
                    System.out.println("connectionLost----------");
                    //startReconnect();
                }
                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {
                    //publish后会执行到这里
                    System.out.println("deliveryComplete---------"
                            + token.isComplete());
                }
                @Override
                public void messageArrived(String topicName, MqttMessage message)
                        throws Exception {
                    //subscribe后得到的消息会执行到这里面
                    System.out.println("messageArrived----------");
                    Message msg = new Message();
                    msg.what = 3;   //收到消息标志位
                    msg.obj = topicName + "---" + message.toString();
                    handler.sendMessage(msg);    // hander 回传
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void Mqtt_connect() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if(!(client.isConnected()) )  //如果还未连接
                    {
                        client.connect(options);
                        Message msg = new Message();
                        msg.what = 31;
                        handler.sendMessage(msg);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    Message msg = new Message();
                    msg.what = 30;
                    handler.sendMessage(msg);
                }
            }
        }).start();
    }

    private void startReconnect() {
        scheduler = Executors.newSingleThreadScheduledExecutor();
        scheduler.scheduleAtFixedRate(new Runnable() {
            @Override
            public void run() {
                if (!client.isConnected()) {
                    Mqtt_connect();
                }
            }
        }, 0 * 1000, 10 * 1000, TimeUnit.MILLISECONDS);
    }

    private void publishmessageplus(String topic,String message2)
    {
        if (client == null || !client.isConnected()) {
            return;
        }
        MqttMessage message = new MqttMessage();
        message.setPayload(message2.getBytes());
        try {
            client.publish(topic,message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}