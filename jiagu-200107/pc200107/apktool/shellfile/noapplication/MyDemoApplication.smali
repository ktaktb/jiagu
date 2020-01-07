.class public Lcom/test/testshell/MyDemoApplication;
.super Landroid/app/Application;
.source "MyDemoApplication.java"


# instance fields
.field app:Landroid/app/Application;

.field loadClassManager:Lcom/example/forshell/ClassLoaderMgr;

.field private mApp:Landroid/app/Application;

.field objProviderlist:Ljava/util/List;


# direct methods
.method public constructor <init>()V
    .locals 1

    .prologue
    .line 22
    invoke-direct {p0}, Landroid/app/Application;-><init>()V

    .line 24
    invoke-static {}, Lcom/example/forshell/ClassLoaderMgr;->getInstance()Lcom/example/forshell/ClassLoaderMgr;

    move-result-object v0

    iput-object v0, p0, Lcom/test/testshell/MyDemoApplication;->loadClassManager:Lcom/example/forshell/ClassLoaderMgr;

    .line 25
    const/4 v0, 0x0

    iput-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    return-void
.end method

.method private InstallProviders(Landroid/app/Application;Ljava/util/List;)Z
    .locals 9
    .param p1, "app"    # Landroid/app/Application;
    .param p2, "providerlist"    # Ljava/util/List;

    .prologue
    const/4 v7, 0x0

    .line 135
    const-string v4, "helloworld"

    const-string v5, "MyDemoApplication--InstallProviders"

    invoke-static {v4, v5}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 136
    const/4 v3, 0x0

    .line 137
    .local v3, "result":Z
    const-string v4, "android.app.ActivityThread"

    const-string v5, "currentActivityThread"

    new-array v6, v7, [Ljava/lang/Class;

    new-array v7, v7, [Ljava/lang/Object;

    invoke-static {v4, v5, v6, v7}, Lcom/example/forshell/Refinvoke;->invokeStaticMethod(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v0

    .line 142
    .local v0, "currentActivityThread":Ljava/lang/Object;
    :try_start_0
    invoke-virtual {v0}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v4

    const-string v5, "installContentProviders"

    const/4 v6, 0x2

    new-array v6, v6, [Ljava/lang/Class;

    const/4 v7, 0x0

    const-class v8, Landroid/content/Context;

    aput-object v8, v6, v7

    const/4 v7, 0x1

    const-class v8, Ljava/util/List;

    aput-object v8, v6, v7

    invoke-virtual {v4, v5, v6}, Ljava/lang/Class;->getDeclaredMethod(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;

    move-result-object v2

    .line 145
    .local v2, "methodinstallContentProviders":Ljava/lang/reflect/Method;
    invoke-virtual {v2}, Ljava/lang/reflect/Method;->isAccessible()Z

    move-result v4

    if-nez v4, :cond_0

    .line 146
    const/4 v4, 0x1

    invoke-virtual {v2, v4}, Ljava/lang/reflect/Method;->setAccessible(Z)V
    :try_end_0
    .catch Ljava/lang/NoSuchMethodException; {:try_start_0 .. :try_end_0} :catch_1

    .line 149
    :cond_0
    const/4 v4, 0x2

    :try_start_1
    new-array v4, v4, [Ljava/lang/Object;

    const/4 v5, 0x0

    aput-object p1, v4, v5

    const/4 v5, 0x1

    aput-object p2, v4, v5

    invoke-virtual {v2, v0, v4}, Ljava/lang/reflect/Method;->invoke(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;
    :try_end_1
    .catch Ljava/lang/IllegalAccessException; {:try_start_1 .. :try_end_1} :catch_0
    .catch Ljava/lang/IllegalArgumentException; {:try_start_1 .. :try_end_1} :catch_2
    .catch Ljava/lang/reflect/InvocationTargetException; {:try_start_1 .. :try_end_1} :catch_3
    .catch Ljava/lang/NoSuchMethodException; {:try_start_1 .. :try_end_1} :catch_1

    .line 151
    const/4 v3, 0x1

    .line 168
    .end local v2    # "methodinstallContentProviders":Ljava/lang/reflect/Method;
    :goto_0
    return v3

    .line 153
    .restart local v2    # "methodinstallContentProviders":Ljava/lang/reflect/Method;
    :catch_0
    move-exception v1

    .line 155
    .local v1, "e":Ljava/lang/IllegalAccessException;
    :try_start_2
    invoke-virtual {v1}, Ljava/lang/IllegalAccessException;->printStackTrace()V
    :try_end_2
    .catch Ljava/lang/NoSuchMethodException; {:try_start_2 .. :try_end_2} :catch_1

    goto :goto_0

    .line 164
    .end local v1    # "e":Ljava/lang/IllegalAccessException;
    .end local v2    # "methodinstallContentProviders":Ljava/lang/reflect/Method;
    :catch_1
    move-exception v1

    .line 166
    .local v1, "e":Ljava/lang/NoSuchMethodException;
    invoke-virtual {v1}, Ljava/lang/NoSuchMethodException;->printStackTrace()V

    goto :goto_0

    .line 156
    .end local v1    # "e":Ljava/lang/NoSuchMethodException;
    .restart local v2    # "methodinstallContentProviders":Ljava/lang/reflect/Method;
    :catch_2
    move-exception v1

    .line 158
    .local v1, "e":Ljava/lang/IllegalArgumentException;
    :try_start_3
    invoke-virtual {v1}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0

    .line 159
    .end local v1    # "e":Ljava/lang/IllegalArgumentException;
    :catch_3
    move-exception v1

    .line 161
    .local v1, "e":Ljava/lang/reflect/InvocationTargetException;
    invoke-virtual {v1}, Ljava/lang/reflect/InvocationTargetException;->printStackTrace()V
    :try_end_3
    .catch Ljava/lang/NoSuchMethodException; {:try_start_3 .. :try_end_3} :catch_1

    goto :goto_0
.end method

.method private SetProviderList(Ljava/lang/Object;)Z
    .locals 10
    .param p1, "objProviderlist"    # Ljava/lang/Object;

    .prologue
    const/4 v9, 0x0

    .line 83
    const-string v6, "helloworld"

    const-string v7, "MyDemoApplication--SetProviderList"

    invoke-static {v6, v7}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 85
    const/4 v5, 0x0

    .line 86
    .local v5, "result":Z
    const-string v6, "android.app.ActivityThread"

    const-string v7, "currentActivityThread"

    new-array v8, v9, [Ljava/lang/Class;

    new-array v9, v9, [Ljava/lang/Object;

    invoke-static {v6, v7, v8, v9}, Lcom/example/forshell/Refinvoke;->invokeStaticMethod(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v0

    .line 91
    .local v0, "currentActivityThread":Ljava/lang/Object;
    :try_start_0
    invoke-virtual {v0}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v6

    const-string v7, "mBoundApplication"

    .line 92
    invoke-virtual {v6, v7}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v2

    .line 93
    .local v2, "fieldBoundApplication":Ljava/lang/reflect/Field;
    invoke-virtual {v2}, Ljava/lang/reflect/Field;->isAccessible()Z

    move-result v6

    if-nez v6, :cond_0

    .line 94
    const/4 v6, 0x1

    invoke-virtual {v2, v6}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 99
    :cond_0
    invoke-virtual {v2, v0}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v4

    .line 101
    .local v4, "objBoundApplication":Ljava/lang/Object;
    invoke-virtual {v4}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v6

    const-string v7, "providers"

    .line 102
    invoke-virtual {v6, v7}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v3

    .line 104
    .local v3, "fieldproviderlist":Ljava/lang/reflect/Field;
    invoke-virtual {v3}, Ljava/lang/reflect/Field;->isAccessible()Z

    move-result v6

    if-nez v6, :cond_1

    .line 105
    const/4 v6, 0x1

    invoke-virtual {v3, v6}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 108
    :cond_1
    invoke-virtual {v3, v4, p1}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V
    :try_end_0
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/NoSuchFieldException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_3

    .line 109
    const/4 v5, 0x1

    .line 131
    .end local v2    # "fieldBoundApplication":Ljava/lang/reflect/Field;
    .end local v3    # "fieldproviderlist":Ljava/lang/reflect/Field;
    .end local v4    # "objBoundApplication":Ljava/lang/Object;
    :goto_0
    return v5

    .line 113
    :catch_0
    move-exception v1

    .line 115
    .local v1, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v1}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_0

    .line 117
    .end local v1    # "e":Ljava/lang/IllegalAccessException;
    :catch_1
    move-exception v1

    .line 119
    .local v1, "e":Ljava/lang/SecurityException;
    invoke-virtual {v1}, Ljava/lang/SecurityException;->printStackTrace()V

    goto :goto_0

    .line 121
    .end local v1    # "e":Ljava/lang/SecurityException;
    :catch_2
    move-exception v1

    .line 123
    .local v1, "e":Ljava/lang/NoSuchFieldException;
    invoke-virtual {v1}, Ljava/lang/NoSuchFieldException;->printStackTrace()V

    goto :goto_0

    .line 125
    .end local v1    # "e":Ljava/lang/NoSuchFieldException;
    :catch_3
    move-exception v1

    .line 127
    .local v1, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v1}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0
.end method


# virtual methods
.method protected attachBaseContext(Landroid/content/Context;)V
    .locals 2
    .param p1, "context"    # Landroid/content/Context;

    .prologue
    .line 219
    invoke-super {p0, p1}, Landroid/app/Application;->attachBaseContext(Landroid/content/Context;)V

    .line 220
    const-string v0, "helloworld"

    const-string v1, "proxy--attachBaseContext start"

    invoke-static {v0, v1}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 221
    invoke-virtual {p0}, Lcom/test/testshell/MyDemoApplication;->emptyProviderlist()Z

    .line 223
    return-void
.end method

.method public emptyProviderlist()Z
    .locals 11

    .prologue
    const/4 v6, 0x1

    const/4 v7, 0x0

    .line 31
    const-string v5, "helloworld"

    const-string v8, "MyDemoApplication--emptyProviderlist()  start"

    invoke-static {v5, v8}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 35
    :try_start_0
    const-string v5, "android.app.ActivityThread"

    const-string v8, "currentActivityThread"

    const/4 v9, 0x0

    new-array v9, v9, [Ljava/lang/Class;

    const/4 v10, 0x0

    new-array v10, v10, [Ljava/lang/Object;

    invoke-static {v5, v8, v9, v10}, Lcom/example/forshell/Refinvoke;->invokeStaticMethod(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v0

    .line 38
    .local v0, "currentActivityThread":Ljava/lang/Object;
    invoke-virtual {v0}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v5

    const-string v8, "mBoundApplication"

    .line 39
    invoke-virtual {v5, v8}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v2

    .line 40
    .local v2, "fieldBoundApplication":Ljava/lang/reflect/Field;
    invoke-virtual {v2}, Ljava/lang/reflect/Field;->isAccessible()Z

    move-result v5

    if-nez v5, :cond_0

    .line 41
    const/4 v5, 0x1

    invoke-virtual {v2, v5}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 46
    :cond_0
    invoke-virtual {v2, v0}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v4

    .line 48
    .local v4, "objBoundApplication":Ljava/lang/Object;
    invoke-virtual {v4}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v5

    const-string v8, "providers"

    .line 49
    invoke-virtual {v5, v8}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v3

    .line 51
    .local v3, "fieldproviderlist":Ljava/lang/reflect/Field;
    invoke-virtual {v3}, Ljava/lang/reflect/Field;->isAccessible()Z

    move-result v5

    if-nez v5, :cond_1

    .line 52
    const/4 v5, 0x1

    invoke-virtual {v3, v5}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 55
    :cond_1
    invoke-virtual {v3, v4}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v5

    check-cast v5, Ljava/util/List;

    iput-object v5, p0, Lcom/test/testshell/MyDemoApplication;->objProviderlist:Ljava/util/List;

    .line 56
    const/4 v5, 0x0

    invoke-virtual {v3, v4, v5}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V
    :try_end_0
    .catch Ljava/lang/IllegalAccessException; {:try_start_0 .. :try_end_0} :catch_0
    .catch Ljava/lang/SecurityException; {:try_start_0 .. :try_end_0} :catch_1
    .catch Ljava/lang/NoSuchFieldException; {:try_start_0 .. :try_end_0} :catch_2
    .catch Ljava/lang/IllegalArgumentException; {:try_start_0 .. :try_end_0} :catch_3

    .line 75
    .end local v0    # "currentActivityThread":Ljava/lang/Object;
    .end local v2    # "fieldBoundApplication":Ljava/lang/reflect/Field;
    .end local v3    # "fieldproviderlist":Ljava/lang/reflect/Field;
    .end local v4    # "objBoundApplication":Ljava/lang/Object;
    :goto_0
    const-string v5, "helloworld"

    new-instance v8, Ljava/lang/StringBuilder;

    invoke-direct {v8}, Ljava/lang/StringBuilder;-><init>()V

    const-string v9, "provider--"

    invoke-virtual {v8, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v8

    iget-object v9, p0, Lcom/test/testshell/MyDemoApplication;->objProviderlist:Ljava/util/List;

    invoke-virtual {v8, v9}, Ljava/lang/StringBuilder;->append(Ljava/lang/Object;)Ljava/lang/StringBuilder;

    move-result-object v8

    invoke-virtual {v8}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v8

    invoke-static {v5, v8}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 76
    iget-object v5, p0, Lcom/test/testshell/MyDemoApplication;->objProviderlist:Ljava/util/List;

    if-eqz v5, :cond_2

    move v5, v6

    .line 79
    :goto_1
    return v5

    .line 58
    :catch_0
    move-exception v1

    .line 60
    .local v1, "e":Ljava/lang/IllegalAccessException;
    invoke-virtual {v1}, Ljava/lang/IllegalAccessException;->printStackTrace()V

    goto :goto_0

    .line 62
    .end local v1    # "e":Ljava/lang/IllegalAccessException;
    :catch_1
    move-exception v1

    .line 64
    .local v1, "e":Ljava/lang/SecurityException;
    invoke-virtual {v1}, Ljava/lang/SecurityException;->printStackTrace()V

    goto :goto_0

    .line 66
    .end local v1    # "e":Ljava/lang/SecurityException;
    :catch_2
    move-exception v1

    .line 68
    .local v1, "e":Ljava/lang/NoSuchFieldException;
    invoke-virtual {v1}, Ljava/lang/NoSuchFieldException;->printStackTrace()V

    goto :goto_0

    .line 70
    .end local v1    # "e":Ljava/lang/NoSuchFieldException;
    :catch_3
    move-exception v1

    .line 72
    .local v1, "e":Ljava/lang/IllegalArgumentException;
    invoke-virtual {v1}, Ljava/lang/IllegalArgumentException;->printStackTrace()V

    goto :goto_0

    .end local v1    # "e":Ljava/lang/IllegalArgumentException;
    :cond_2
    move v5, v7

    .line 79
    goto :goto_1
.end method

.method public onConfigurationChanged(Landroid/content/res/Configuration;)V
    .locals 2
    .param p1, "newConfig"    # Landroid/content/res/Configuration;

    .prologue
    .line 227
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v1, "onConfigurationChanged "

    invoke-virtual {v0, v1}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 229
    iget-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    if-eqz v0, :cond_0

    .line 230
    iget-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    invoke-virtual {v0, p1}, Landroid/app/Application;->onConfigurationChanged(Landroid/content/res/Configuration;)V

    .line 232
    :cond_0
    return-void
.end method

.method public onCreate()V
    .locals 12

    .prologue
    .line 174
    const-string v10, "helloworld"

    const-string v11, "MyDemoApplication--onCreate begin"

    invoke-static {v10, v11}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 178
    :try_start_0
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->loadClassManager:Lcom/example/forshell/ClassLoaderMgr;

    .line 179
    invoke-virtual {p0}, Lcom/test/testshell/MyDemoApplication;->getResources()Landroid/content/res/Resources;

    move-result-object v11

    invoke-virtual {v11}, Landroid/content/res/Resources;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v11

    .line 178
    invoke-virtual {v10, v11, p0}, Lcom/example/forshell/ClassLoaderMgr;->setClassLoader(Landroid/content/res/AssetManager;Landroid/content/Context;)Ljava/lang/ClassLoader;
    :try_end_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_0

    move-result-object v6

    .line 191
    :goto_0
    :try_start_1
    const-class v10, Landroid/content/ContextWrapper;

    const-string v11, "mBase"

    invoke-static {v10, v11}, Lcom/example/forshell/Smith;->getField(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v9

    .line 193
    .local v9, "mNewBase":Ljava/lang/reflect/Field;
    invoke-virtual {p0}, Lcom/test/testshell/MyDemoApplication;->getApplicationContext()Landroid/content/Context;

    move-result-object v10

    .line 192
    invoke-virtual {v9, v10}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v3

    check-cast v3, Landroid/content/Context;

    .line 194
    .local v3, "context":Landroid/content/Context;
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->app:Landroid/app/Application;

    invoke-virtual {v9, v10, v3}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V

    .line 195
    invoke-virtual {v3}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v10

    const-string v11, "mPackageInfo"

    invoke-static {v10, v11}, Lcom/example/forshell/Smith;->getField(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v0

    .line 196
    .local v0, "apk":Ljava/lang/reflect/Field;
    invoke-virtual {v0, v3}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;

    move-result-object v2

    .line 197
    .local v2, "apkObj":Ljava/lang/Object;
    invoke-virtual {v2}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v1

    .line 198
    .local v1, "apkClass":Ljava/lang/Class;, "Ljava/lang/Class<*>;"
    sget v10, Landroid/os/Build$VERSION;->SDK_INT:I

    const/16 v11, 0xe

    if-lt v10, v11, :cond_0

    .line 199
    const-class v10, Landroid/app/Application;

    const-string v11, "mLoadedApk"

    invoke-static {v10, v11}, Lcom/example/forshell/Smith;->getField(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v8

    .line 201
    .local v8, "mLoadedApk":Ljava/lang/reflect/Field;
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->app:Landroid/app/Application;

    invoke-virtual {v8, v10, v2}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V

    .line 203
    .end local v8    # "mLoadedApk":Ljava/lang/reflect/Field;
    :cond_0
    const-string v10, "mApplication"

    invoke-static {v1, v10}, Lcom/example/forshell/Smith;->getField(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v7

    .line 204
    .local v7, "mApplication":Ljava/lang/reflect/Field;
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->app:Landroid/app/Application;

    invoke-virtual {v7, v2, v10}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V

    .line 206
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->app:Landroid/app/Application;

    iput-object v10, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    .line 207
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->app:Landroid/app/Application;

    invoke-virtual {v10}, Landroid/app/Application;->onCreate()V
    :try_end_1
    .catch Ljava/lang/Exception; {:try_start_1 .. :try_end_1} :catch_1

    .line 213
    .end local v0    # "apk":Ljava/lang/reflect/Field;
    .end local v1    # "apkClass":Ljava/lang/Class;, "Ljava/lang/Class<*>;"
    .end local v2    # "apkObj":Ljava/lang/Object;
    .end local v3    # "context":Landroid/content/Context;
    .end local v7    # "mApplication":Ljava/lang/reflect/Field;
    .end local v9    # "mNewBase":Ljava/lang/reflect/Field;
    :goto_1
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->objProviderlist:Ljava/util/List;

    invoke-direct {p0, v10}, Lcom/test/testshell/MyDemoApplication;->SetProviderList(Ljava/lang/Object;)Z

    .line 214
    iget-object v10, p0, Lcom/test/testshell/MyDemoApplication;->objProviderlist:Ljava/util/List;

    invoke-direct {p0, p0, v10}, Lcom/test/testshell/MyDemoApplication;->InstallProviders(Landroid/app/Application;Ljava/util/List;)Z

    .line 215
    return-void

    .line 180
    :catch_0
    move-exception v5

    .line 182
    .local v5, "e1":Ljava/io/IOException;
    invoke-virtual {v5}, Ljava/io/IOException;->printStackTrace()V

    goto :goto_0

    .line 208
    .end local v5    # "e1":Ljava/io/IOException;
    :catch_1
    move-exception v4

    .line 210
    .local v4, "e":Ljava/lang/Exception;
    const-string v10, "helloworld"

    const-string v11, "MyDemoApplication--onCreate exception"

    invoke-static {v10, v11}, Landroid/util/Log;->v(Ljava/lang/String;Ljava/lang/String;)I

    .line 211
    invoke-virtual {v4}, Ljava/lang/Exception;->printStackTrace()V

    goto :goto_1
.end method

.method public onLowMemory()V
    .locals 2

    .prologue
    .line 236
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v1, "onLowMemory "

    invoke-virtual {v0, v1}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 238
    iget-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    if-eqz v0, :cond_0

    .line 239
    iget-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    invoke-virtual {v0}, Landroid/app/Application;->onLowMemory()V

    .line 241
    :cond_0
    return-void
.end method

.method public onTerminate()V
    .locals 2

    .prologue
    .line 245
    sget-object v0, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v1, "onTerminate "

    invoke-virtual {v0, v1}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 247
    iget-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    if-eqz v0, :cond_0

    .line 248
    iget-object v0, p0, Lcom/test/testshell/MyDemoApplication;->mApp:Landroid/app/Application;

    invoke-virtual {v0}, Landroid/app/Application;->onTerminate()V

    .line 250
    :cond_0
    return-void
.end method
