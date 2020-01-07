package com.test.testshell;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.io.IOException;

import com.example.forshell.ClassLoaderMgr;
import com.example.forshell.Refinvoke;
import com.example.forshell.Smith;
import java.lang.reflect.*;

import android.app.Application;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;

public class MyDemoApplication extends Application {

	ClassLoaderMgr loadClassManager = ClassLoaderMgr.getInstance();
	private Application mApp = null;
	List objProviderlist;
	Application app;

	public boolean emptyProviderlist() {

		Log.i("helloworld", "MyDemoApplication--emptyProviderlist()  start");
		try {
			// set the mOuterConterxt application as the delegate application
			// object
			Object currentActivityThread = Refinvoke.invokeStaticMethod(
					"android.app.ActivityThread", "currentActivityThread",
					new Class[] {}, new Object[] {});
			Field fieldBoundApplication = currentActivityThread.getClass()
					.getDeclaredField("mBoundApplication");
			if (!fieldBoundApplication.isAccessible()) {
				fieldBoundApplication.setAccessible(true);
			}
			// Object objInitialApplication =
			// fieldInitialApplication.get(objActivityThread);
			Object objBoundApplication = fieldBoundApplication
					.get(currentActivityThread);

			Field fieldproviderlist = objBoundApplication.getClass()
					.getDeclaredField("providers");

			if (!fieldproviderlist.isAccessible()) {
				fieldproviderlist.setAccessible(true);
			}

			objProviderlist = (List) fieldproviderlist.get(objBoundApplication);
			fieldproviderlist.set(objBoundApplication, null);

		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (NoSuchFieldException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		}
		Log.i("helloworld", "provider--" + objProviderlist);
		if (objProviderlist != null)
			return true;
		else
			return false;
	}

	private boolean SetProviderList(Object objProviderlist) {
		Log.i("helloworld", "MyDemoApplication--SetProviderList");

		boolean result = false;
		Object currentActivityThread = Refinvoke.invokeStaticMethod(
				"android.app.ActivityThread", "currentActivityThread",
				new Class[] {}, new Object[] {});
		try {

			Field fieldBoundApplication = currentActivityThread.getClass()
					.getDeclaredField("mBoundApplication");
			if (!fieldBoundApplication.isAccessible()) {
				fieldBoundApplication.setAccessible(true);
			}
			// Object objInitialApplication =
			// fieldInitialApplication.get(objActivityThread);
			Object objBoundApplication = fieldBoundApplication
					.get(currentActivityThread);

			Field fieldproviderlist = objBoundApplication.getClass()
					.getDeclaredField("providers");

			if (!fieldproviderlist.isAccessible()) {
				fieldproviderlist.setAccessible(true);
			}

			fieldproviderlist.set(objBoundApplication, objProviderlist);
			result = true;

		}

		catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (NoSuchFieldException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		}

		return result;
	}

	private boolean InstallProviders(Application app, List providerlist) {
		Log.i("helloworld", "MyDemoApplication--InstallProviders");
		boolean result = false;
		Object currentActivityThread = Refinvoke.invokeStaticMethod(
				"android.app.ActivityThread", "currentActivityThread",
				new Class[] {}, new Object[] {});
		try {
			Method methodinstallContentProviders = currentActivityThread
					.getClass().getDeclaredMethod("installContentProviders",
							Context.class, List.class);

			if (!methodinstallContentProviders.isAccessible()) {
				methodinstallContentProviders.setAccessible(true);
			}
			try {
				methodinstallContentProviders.invoke(currentActivityThread,
						app, providerlist);
				result = true;

			} catch (IllegalAccessException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return result;
	}

	@Override
	public void onCreate() {

		Log.i("helloworld", "MyDemoApplication--onCreate begin");

		ClassLoader localDexClassLoader;
		try {
			localDexClassLoader = loadClassManager.setClassLoader(
					getResources().getAssets(), this);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		 //System.loadLibrary("sec");
		 try {
		
//		 app = (Application)
//                     this.getClass().getClassLoader().loadClass("com.test.testshell.MyApplication").newInstance();
//             // 设置context的application信息为app
             Field mNewBase = Smith.getField(ContextWrapper.class, "mBase");
             Context context = (Context) mNewBase.get(this
                     .getApplicationContext());
             mNewBase.set(app, context);
             Field apk = Smith.getField(context.getClass(), "mPackageInfo");
             Object apkObj = apk.get(context);
             Class<?> apkClass = apkObj.getClass();
             if (android.os.Build.VERSION.SDK_INT >= 14) {
                 Field mLoadedApk = Smith.getField(Application.class,
                         "mLoadedApk");
                 mLoadedApk.set(app, apkObj);
             }
             Field mApplication = Smith.getField(apkClass, "mApplication");
             mApplication.set(apkObj, (Application) app);
             // 设置context的application信息为app结束。
             mApp = app;
             app.onCreate();
		 } catch (Exception e) {
		 // TODO: handle exception
		 Log.v("helloworld", "MyDemoApplication--onCreate exception");
		 e.printStackTrace();
		 }
		SetProviderList(objProviderlist);
		InstallProviders(this, objProviderlist);
	}

	@Override
	protected void attachBaseContext(Context context) {
		super.attachBaseContext(context);
	    Log.i("helloworld", "proxy--attachBaseContext start");
		emptyProviderlist();

	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
        System.out.println("onConfigurationChanged ");
//		super.onConfigurationChanged(newConfig);
		if (mApp != null) {
			mApp.onConfigurationChanged(newConfig);
		}
	}

	@Override
	public void onLowMemory() {
        System.out.println("onLowMemory ");
//		super.onLowMemory();
		if (mApp != null) {
			mApp.onLowMemory();
		}
	}

	@Override
	public void onTerminate() {
        System.out.println("onTerminate ");
//        super.onTerminate();
		if (mApp != null) {
			mApp.onTerminate();
		}
	}

}