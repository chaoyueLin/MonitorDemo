package com.example.monitordemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.TextView

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        DeadLockUtil.createDeadLock()
        // Example of a call to a native method
        findViewById<TextView>(R.id.sample_text).text = stringFromJNI()

        findViewById<Button>(R.id.button).setOnClickListener {
            DeadLockMonitor().startMonitor()
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}