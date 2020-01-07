.class public Lcom/example/forshell/ClassLoaderMgr;
.super Ljava/lang/Object;
.source "ClassLoaderMgr.java"


# static fields
.field static localDexClassLoader:Ljava/lang/ClassLoader;

.field static myInstance:Lcom/example/forshell/ClassLoaderMgr;


# instance fields
.field assetManager:Landroid/content/res/AssetManager;

.field public flag:Z


# direct methods
.method static constructor <clinit>()V
    .locals 1

    .prologue
    const/4 v0, 0x0

    .line 27
    sput-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    .line 28
    sput-object v0, Lcom/example/forshell/ClassLoaderMgr;->localDexClassLoader:Ljava/lang/ClassLoader;

    return-void
.end method

.method private constructor <init>()V
    .locals 1

    .prologue
    .line 32
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    .line 29
    const/4 v0, 0x0

    iput-boolean v0, p0, Lcom/example/forshell/ClassLoaderMgr;->flag:Z

    .line 34
    return-void
.end method

.method public static getInstance()Lcom/example/forshell/ClassLoaderMgr;
    .locals 1

    .prologue
    .line 37
    sget-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    if-nez v0, :cond_0

    .line 39
    new-instance v0, Lcom/example/forshell/ClassLoaderMgr;

    invoke-direct {v0}, Lcom/example/forshell/ClassLoaderMgr;-><init>()V

    sput-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    .line 42
    :cond_0
    sget-object v0, Lcom/example/forshell/ClassLoaderMgr;->myInstance:Lcom/example/forshell/ClassLoaderMgr;

    return-object v0
.end method


# virtual methods
.method public setClassLoader(Landroid/content/res/AssetManager;Landroid/content/Context;)Ljava/lang/ClassLoader;
    .locals 36
    .param p1, "assetManager"    # Landroid/content/res/AssetManager;
    .param p2, "ctx"    # Landroid/content/Context;
    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/io/IOException;
        }
    .end annotation

    .prologue
    .line 47
    invoke-virtual/range {p0 .. p0}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/Class;->getClassLoader()Ljava/lang/ClassLoader;

    move-result-object v29

    .line 48
    .local v29, "myLoader":Ljava/lang/ClassLoader;
    invoke-virtual/range {v29 .. v29}, Ljava/lang/ClassLoader;->getParent()Ljava/lang/ClassLoader;

    move-result-object v31

    .line 50
    .local v31, "parent":Ljava/lang/ClassLoader;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    invoke-virtual/range {v31 .. v31}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v35

    invoke-virtual/range {v35 .. v35}, Ljava/lang/Class;->getName()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 52
    const/16 v26, 0x0

    .line 53
    .local v26, "is":Ljava/io/InputStream;
    const/16 v23, 0x0

    .line 54
    .local v23, "fos":Ljava/io/FileOutputStream;
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getFilesDir()Ljava/io/File;

    move-result-object v22

    .line 55
    .local v22, "filefir":Ljava/io/File;
    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v7

    .line 56
    .local v7, "datapath":Ljava/lang/String;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    move-object/from16 v0, v34

    invoke-virtual {v0, v7}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 57
    const/16 v34, 0x0

    invoke-virtual {v7}, Ljava/lang/String;->length()I

    move-result v35

    add-int/lit8 v35, v35, -0x6

    move/from16 v0, v34

    move/from16 v1, v35

    invoke-virtual {v7, v0, v1}, Ljava/lang/String;->substring(II)Ljava/lang/String;

    move-result-object v7

    .line 61
    move-object/from16 v0, p0

    iget-boolean v0, v0, Lcom/example/forshell/ClassLoaderMgr;->flag:Z

    move/from16 v34, v0

    if-nez v34, :cond_f

    .line 88
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/class.bin"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 89
    .local v21, "fileName":Ljava/lang/String;
    new-instance v14, Ljava/io/File;

    move-object/from16 v0, v21

    invoke-direct {v14, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 90
    .local v14, "file2":Ljava/io/File;
    invoke-virtual {v14}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_0

    .line 92
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "upgrate bin file"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 95
    :cond_0
    invoke-virtual {v14}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_1

    .line 96
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy bin files"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 98
    :try_start_0
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "class.bin"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_0
    .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_1

    move-result-object v26

    .line 100
    :try_start_1
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 101
    .local v6, "count":I
    new-array v3, v6, [B

    .line 102
    .local v3, "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 103
    const-string v34, "class.bin"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 104
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 105
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 106
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_1
    .catch Ljava/io/FileNotFoundException; {:try_start_1 .. :try_end_1} :catch_0
    .catch Ljava/io/IOException; {:try_start_1 .. :try_end_1} :catch_1

    .line 117
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_1
    :goto_0
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/classes.dex"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 118
    new-instance v17, Ljava/io/File;

    move-object/from16 v0, v17

    move-object/from16 v1, v21

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 119
    .local v17, "file5":Ljava/io/File;
    invoke-virtual/range {v17 .. v17}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_2

    .line 122
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "upgrate dex file"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 125
    :cond_2
    invoke-virtual/range {v17 .. v17}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_3

    .line 126
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy dex files"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 128
    :try_start_2
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "classes.dex"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_2
    .catch Ljava/io/IOException; {:try_start_2 .. :try_end_2} :catch_3

    move-result-object v26

    .line 130
    :try_start_3
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 131
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 132
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 133
    const-string v34, "haha.dex"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 135
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 136
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 137
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_3
    .catch Ljava/io/FileNotFoundException; {:try_start_3 .. :try_end_3} :catch_2
    .catch Ljava/io/IOException; {:try_start_3 .. :try_end_3} :catch_3

    .line 183
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_3
    :goto_1
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/config.bin"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 184
    new-instance v20, Ljava/io/File;

    invoke-direct/range {v20 .. v21}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 185
    .local v20, "file99":Ljava/io/File;
    invoke-virtual/range {v20 .. v20}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_4

    .line 188
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "upgrate config.bin file"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 191
    :cond_4
    invoke-virtual/range {v20 .. v20}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_5

    .line 192
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy config.bin files"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 194
    :try_start_4
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "config.bin"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_4
    .catch Ljava/io/IOException; {:try_start_4 .. :try_end_4} :catch_5

    move-result-object v26

    .line 196
    :try_start_5
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 197
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 198
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 199
    const-string v34, "config.bin"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 201
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 202
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 203
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_5
    .catch Ljava/io/FileNotFoundException; {:try_start_5 .. :try_end_5} :catch_4
    .catch Ljava/io/IOException; {:try_start_5 .. :try_end_5} :catch_5

    .line 215
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_5
    :goto_2
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/codedata2.txt"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 216
    new-instance v10, Ljava/io/File;

    move-object/from16 v0, v21

    invoke-direct {v10, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 217
    .local v10, "file11":Ljava/io/File;
    invoke-virtual {v10}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_6

    .line 220
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "upgrate codedata.txt file"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 223
    :cond_6
    invoke-virtual {v10}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_7

    .line 224
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy codedata files"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 226
    :try_start_6
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "codedata2.txt"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_6
    .catch Ljava/io/IOException; {:try_start_6 .. :try_end_6} :catch_7

    move-result-object v26

    .line 228
    :try_start_7
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 229
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 230
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 231
    const-string v34, "codedata.txt"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 233
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 234
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 235
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_7
    .catch Ljava/io/FileNotFoundException; {:try_start_7 .. :try_end_7} :catch_6
    .catch Ljava/io/IOException; {:try_start_7 .. :try_end_7} :catch_7

    .line 284
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_7
    :goto_3
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/codeinfo2.txt"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 285
    new-instance v11, Ljava/io/File;

    move-object/from16 v0, v21

    invoke-direct {v11, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 286
    .local v11, "file12":Ljava/io/File;
    invoke-virtual {v11}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_8

    .line 289
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "upgrate codeinfo.txt file"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 292
    :cond_8
    invoke-virtual {v11}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_9

    .line 293
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy codeinfo.txt files"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 295
    :try_start_8
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "codeinfo2.txt"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_8
    .catch Ljava/io/IOException; {:try_start_8 .. :try_end_8} :catch_9

    move-result-object v26

    .line 297
    :try_start_9
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 298
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 299
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 300
    const-string v34, "codeinfo.txt"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 302
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 303
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 304
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_9
    .catch Ljava/io/FileNotFoundException; {:try_start_9 .. :try_end_9} :catch_8
    .catch Ljava/io/IOException; {:try_start_9 .. :try_end_9} :catch_9

    .line 318
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_9
    :goto_4
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/inv_M.m"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 319
    new-instance v18, Ljava/io/File;

    move-object/from16 v0, v18

    move-object/from16 v1, v21

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 320
    .local v18, "file8":Ljava/io/File;
    invoke-virtual/range {v18 .. v18}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_a

    .line 323
    :try_start_a
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "inv_M.m"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_a
    .catch Ljava/lang/Exception; {:try_start_a .. :try_end_a} :catch_b

    move-result-object v26

    .line 325
    :try_start_b
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 326
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 327
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 328
    const-string v34, "inv_M.m"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 330
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 331
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 332
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_b
    .catch Ljava/lang/Exception; {:try_start_b .. :try_end_b} :catch_a

    .line 340
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_a
    :goto_5
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/inv_tables.t"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 341
    new-instance v19, Ljava/io/File;

    move-object/from16 v0, v19

    move-object/from16 v1, v21

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 342
    .local v19, "file9":Ljava/io/File;
    invoke-virtual/range {v19 .. v19}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_b

    .line 345
    :try_start_c
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "inv_tables.t"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_c
    .catch Ljava/lang/Exception; {:try_start_c .. :try_end_c} :catch_d

    move-result-object v26

    .line 347
    :try_start_d
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 348
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 349
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 350
    const-string v34, "inv_tables.t"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 352
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 353
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 354
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_d
    .catch Ljava/lang/Exception; {:try_start_d .. :try_end_d} :catch_c

    .line 362
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_b
    :goto_6
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/soname.txt"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 363
    new-instance v12, Ljava/io/File;

    move-object/from16 v0, v21

    invoke-direct {v12, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 364
    .local v12, "file18":Ljava/io/File;
    invoke-virtual {v12}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_c

    .line 367
    :try_start_e
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "soname.txt"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_e
    .catch Ljava/lang/Exception; {:try_start_e .. :try_end_e} :catch_f

    move-result-object v26

    .line 369
    :try_start_f
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 370
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 371
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 372
    const-string v34, "soname.txt"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 374
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 375
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 376
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_f
    .catch Ljava/lang/Exception; {:try_start_f .. :try_end_f} :catch_e

    .line 384
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_c
    :goto_7
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/lib.bin"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 385
    new-instance v13, Ljava/io/File;

    move-object/from16 v0, v21

    invoke-direct {v13, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 386
    .local v13, "file19":Ljava/io/File;
    invoke-virtual {v13}, Ljava/io/File;->exists()Z

    move-result v34

    if-nez v34, :cond_d

    .line 389
    :try_start_10
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getAssets()Landroid/content/res/AssetManager;

    move-result-object v34

    const-string v35, "lib.bin"

    invoke-virtual/range {v34 .. v35}, Landroid/content/res/AssetManager;->open(Ljava/lang/String;)Ljava/io/InputStream;
    :try_end_10
    .catch Ljava/lang/Exception; {:try_start_10 .. :try_end_10} :catch_11

    move-result-object v26

    .line 391
    :try_start_11
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->available()I

    move-result v6

    .line 392
    .restart local v6    # "count":I
    new-array v3, v6, [B

    .line 393
    .restart local v3    # "b":[B
    move-object/from16 v0, v26

    invoke-virtual {v0, v3}, Ljava/io/InputStream;->read([B)I

    .line 394
    const-string v34, "lib.bin"

    const/16 v35, 0x0

    move-object/from16 v0, p2

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/Context;->openFileOutput(Ljava/lang/String;I)Ljava/io/FileOutputStream;

    move-result-object v23

    .line 396
    move-object/from16 v0, v23

    invoke-virtual {v0, v3}, Ljava/io/FileOutputStream;->write([B)V

    .line 397
    invoke-virtual/range {v23 .. v23}, Ljava/io/FileOutputStream;->close()V

    .line 398
    invoke-virtual/range {v26 .. v26}, Ljava/io/InputStream;->close()V
    :try_end_11
    .catch Ljava/lang/Exception; {:try_start_11 .. :try_end_11} :catch_10

    .line 430
    .end local v3    # "b":[B
    .end local v6    # "count":I
    :cond_d
    :goto_8
    :try_start_12
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getPackageManager()Landroid/content/pm/PackageManager;

    move-result-object v28

    .line 431
    .local v28, "manager":Landroid/content/pm/PackageManager;
    invoke-virtual/range {p2 .. p2}, Landroid/content/Context;->getPackageName()Ljava/lang/String;

    move-result-object v34

    const/16 v35, 0x0

    move-object/from16 v0, v28

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v1, v2}, Landroid/content/pm/PackageManager;->getPackageInfo(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;

    move-result-object v25

    .line 433
    .local v25, "info":Landroid/content/pm/PackageInfo;
    move-object/from16 v0, v25

    iget-object v0, v0, Landroid/content/pm/PackageInfo;->versionName:Ljava/lang/String;

    move-object/from16 v33, v0

    .line 434
    .local v33, "version":Ljava/lang/String;
    const/16 v34, 0x64

    move/from16 v0, v34

    new-array v4, v0, [B

    .line 435
    .local v4, "bt":[B
    invoke-virtual/range {v33 .. v33}, Ljava/lang/String;->getBytes()[B

    move-result-object v4

    .line 438
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/versionFile"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 439
    new-instance v15, Ljava/io/File;

    move-object/from16 v0, v21

    invoke-direct {v15, v0}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 440
    .local v15, "file3":Ljava/io/File;
    invoke-virtual {v15}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_10

    .line 442
    const-string v32, ""
    :try_end_12
    .catch Ljava/lang/Exception; {:try_start_12 .. :try_end_12} :catch_14

    .line 444
    .local v32, "ver":Ljava/lang/String;
    :try_start_13
    new-instance v30, Ljava/io/FileInputStream;

    .line 445
    invoke-virtual {v15}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    move-object/from16 v0, v30

    move-object/from16 v1, v34

    invoke-direct {v0, v1}, Ljava/io/FileInputStream;-><init>(Ljava/lang/String;)V

    .line 446
    .local v30, "out":Ljava/io/FileInputStream;
    invoke-virtual/range {v30 .. v30}, Ljava/io/FileInputStream;->available()I

    move-result v27

    .line 447
    .local v27, "lenght":I
    move/from16 v0, v27

    new-array v5, v0, [B

    .line 448
    .local v5, "buffer":[B
    const/16 v34, 0x0

    move-object/from16 v0, v30

    move/from16 v1, v34

    move/from16 v2, v27

    invoke-virtual {v0, v5, v1, v2}, Ljava/io/FileInputStream;->read([BII)I

    .line 449
    invoke-virtual/range {v30 .. v30}, Ljava/io/FileInputStream;->close()V

    .line 450
    const-string v34, "UTF-8"

    move-object/from16 v0, v34

    invoke-static {v5, v0}, Lorg/apache/http/util/EncodingUtils;->getString([BLjava/lang/String;)Ljava/lang/String;

    move-result-object v32

    .line 452
    invoke-virtual/range {v32 .. v32}, Ljava/lang/String;->intern()Ljava/lang/String;

    move-result-object v34

    invoke-virtual/range {v33 .. v33}, Ljava/lang/String;->intern()Ljava/lang/String;

    move-result-object v35

    move-object/from16 v0, v34

    move-object/from16 v1, v35

    if-eq v0, v1, :cond_e

    .line 455
    invoke-virtual {v15}, Ljava/io/File;->delete()Z

    .line 456
    new-instance v24, Ljava/io/FileOutputStream;

    .line 457
    invoke-virtual {v15}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    const/16 v35, 0x1

    move-object/from16 v0, v24

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-direct {v0, v1, v2}, Ljava/io/FileOutputStream;-><init>(Ljava/lang/String;Z)V
    :try_end_13
    .catch Ljava/lang/Exception; {:try_start_13 .. :try_end_13} :catch_13

    .line 459
    .local v24, "in":Ljava/io/FileOutputStream;
    const/16 v34, 0x0

    :try_start_14
    array-length v0, v4

    move/from16 v35, v0

    move-object/from16 v0, v24

    move/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v4, v1, v2}, Ljava/io/FileOutputStream;->write([BII)V

    .line 460
    invoke-virtual/range {v24 .. v24}, Ljava/io/FileOutputStream;->close()V
    :try_end_14
    .catch Ljava/io/IOException; {:try_start_14 .. :try_end_14} :catch_12
    .catch Ljava/lang/Exception; {:try_start_14 .. :try_end_14} :catch_13

    .line 465
    :goto_9
    :try_start_15
    new-instance v34, Ljava/lang/StringBuilder;

    invoke-direct/range {v34 .. v34}, Ljava/lang/StringBuilder;-><init>()V

    invoke-virtual/range {v22 .. v22}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v35

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    const-string v35, "/dexflag.flag"

    invoke-virtual/range {v34 .. v35}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v34

    invoke-virtual/range {v34 .. v34}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v21

    .line 467
    new-instance v16, Ljava/io/File;

    move-object/from16 v0, v16

    move-object/from16 v1, v21

    invoke-direct {v0, v1}, Ljava/io/File;-><init>(Ljava/lang/String;)V

    .line 468
    .local v16, "file4":Ljava/io/File;
    invoke-virtual/range {v16 .. v16}, Ljava/io/File;->exists()Z

    move-result v34

    if-eqz v34, :cond_e

    .line 469
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "upgrate fake dex file"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 470
    invoke-virtual/range {v16 .. v16}, Ljava/io/File;->delete()Z
    :try_end_15
    .catch Ljava/lang/Exception; {:try_start_15 .. :try_end_15} :catch_13

    .line 492
    .end local v4    # "bt":[B
    .end local v5    # "buffer":[B
    .end local v15    # "file3":Ljava/io/File;
    .end local v16    # "file4":Ljava/io/File;
    .end local v24    # "in":Ljava/io/FileOutputStream;
    .end local v25    # "info":Landroid/content/pm/PackageInfo;
    .end local v27    # "lenght":I
    .end local v28    # "manager":Landroid/content/pm/PackageManager;
    .end local v30    # "out":Ljava/io/FileInputStream;
    .end local v32    # "ver":Ljava/lang/String;
    .end local v33    # "version":Ljava/lang/String;
    :cond_e
    :goto_a
    const-string v34, "sec"

    invoke-static/range {v34 .. v34}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

    .line 496
    .end local v10    # "file11":Ljava/io/File;
    .end local v11    # "file12":Ljava/io/File;
    .end local v12    # "file18":Ljava/io/File;
    .end local v13    # "file19":Ljava/io/File;
    .end local v14    # "file2":Ljava/io/File;
    .end local v17    # "file5":Ljava/io/File;
    .end local v18    # "file8":Ljava/io/File;
    .end local v19    # "file9":Ljava/io/File;
    .end local v20    # "file99":Ljava/io/File;
    .end local v21    # "fileName":Ljava/lang/String;
    :cond_f
    sget-object v34, Lcom/example/forshell/ClassLoaderMgr;->localDexClassLoader:Ljava/lang/ClassLoader;

    return-object v34

    .line 107
    .restart local v14    # "file2":Ljava/io/File;
    .restart local v21    # "fileName":Ljava/lang/String;
    :catch_0
    move-exception v8

    .line 108
    .local v8, "e":Ljava/io/FileNotFoundException;
    :try_start_16
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 109
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_16
    .catch Ljava/io/IOException; {:try_start_16 .. :try_end_16} :catch_1

    goto/16 :goto_0

    .line 111
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_1
    move-exception v9

    .line 113
    .local v9, "e1":Ljava/io/IOException;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "open files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 114
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_0

    .line 138
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v17    # "file5":Ljava/io/File;
    :catch_2
    move-exception v8

    .line 139
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_17
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 140
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_17
    .catch Ljava/io/IOException; {:try_start_17 .. :try_end_17} :catch_3

    goto/16 :goto_1

    .line 142
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_3
    move-exception v9

    .line 144
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "open files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 145
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_1

    .line 204
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v20    # "file99":Ljava/io/File;
    :catch_4
    move-exception v8

    .line 205
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_18
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 206
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_18
    .catch Ljava/io/IOException; {:try_start_18 .. :try_end_18} :catch_5

    goto/16 :goto_2

    .line 208
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_5
    move-exception v9

    .line 210
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "open files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 211
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_2

    .line 236
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v10    # "file11":Ljava/io/File;
    :catch_6
    move-exception v8

    .line 237
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_19
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 238
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_19
    .catch Ljava/io/IOException; {:try_start_19 .. :try_end_19} :catch_7

    goto/16 :goto_3

    .line 240
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_7
    move-exception v9

    .line 242
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "open files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 243
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_3

    .line 305
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v11    # "file12":Ljava/io/File;
    :catch_8
    move-exception v8

    .line 306
    .restart local v8    # "e":Ljava/io/FileNotFoundException;
    :try_start_1a
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "copy files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 307
    invoke-virtual {v8}, Ljava/io/FileNotFoundException;->printStackTrace()V
    :try_end_1a
    .catch Ljava/io/IOException; {:try_start_1a .. :try_end_1a} :catch_9

    goto/16 :goto_4

    .line 309
    .end local v8    # "e":Ljava/io/FileNotFoundException;
    :catch_9
    move-exception v9

    .line 311
    .restart local v9    # "e1":Ljava/io/IOException;
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "open files error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    .line 312
    invoke-virtual {v9}, Ljava/io/IOException;->printStackTrace()V

    goto/16 :goto_4

    .line 333
    .end local v9    # "e1":Ljava/io/IOException;
    .restart local v18    # "file8":Ljava/io/File;
    :catch_a
    move-exception v8

    .line 334
    .local v8, "e":Ljava/lang/Exception;
    :try_start_1b
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1b
    .catch Ljava/lang/Exception; {:try_start_1b .. :try_end_1b} :catch_b

    goto/16 :goto_5

    .line 336
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_b
    move-exception v8

    .line 337
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_5

    .line 355
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v19    # "file9":Ljava/io/File;
    :catch_c
    move-exception v8

    .line 356
    .restart local v8    # "e":Ljava/lang/Exception;
    :try_start_1c
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1c
    .catch Ljava/lang/Exception; {:try_start_1c .. :try_end_1c} :catch_d

    goto/16 :goto_6

    .line 358
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_d
    move-exception v8

    .line 359
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_6

    .line 377
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v12    # "file18":Ljava/io/File;
    :catch_e
    move-exception v8

    .line 378
    .restart local v8    # "e":Ljava/lang/Exception;
    :try_start_1d
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1d
    .catch Ljava/lang/Exception; {:try_start_1d .. :try_end_1d} :catch_f

    goto/16 :goto_7

    .line 380
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_f
    move-exception v8

    .line 381
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_7

    .line 399
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v13    # "file19":Ljava/io/File;
    :catch_10
    move-exception v8

    .line 400
    .restart local v8    # "e":Ljava/lang/Exception;
    :try_start_1e
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_1e
    .catch Ljava/lang/Exception; {:try_start_1e .. :try_end_1e} :catch_11

    goto/16 :goto_8

    .line 402
    .end local v8    # "e":Ljava/lang/Exception;
    :catch_11
    move-exception v8

    .line 403
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    goto/16 :goto_8

    .line 461
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v4    # "bt":[B
    .restart local v5    # "buffer":[B
    .restart local v15    # "file3":Ljava/io/File;
    .restart local v24    # "in":Ljava/io/FileOutputStream;
    .restart local v25    # "info":Landroid/content/pm/PackageInfo;
    .restart local v27    # "lenght":I
    .restart local v28    # "manager":Landroid/content/pm/PackageManager;
    .restart local v30    # "out":Ljava/io/FileInputStream;
    .restart local v32    # "ver":Ljava/lang/String;
    .restart local v33    # "version":Ljava/lang/String;
    :catch_12
    move-exception v8

    .line 462
    .local v8, "e":Ljava/io/IOException;
    :try_start_1f
    invoke-virtual {v8}, Ljava/io/IOException;->printStackTrace()V
    :try_end_1f
    .catch Ljava/lang/Exception; {:try_start_1f .. :try_end_1f} :catch_13

    goto/16 :goto_9

    .line 473
    .end local v5    # "buffer":[B
    .end local v8    # "e":Ljava/io/IOException;
    .end local v24    # "in":Ljava/io/FileOutputStream;
    .end local v27    # "lenght":I
    .end local v30    # "out":Ljava/io/FileInputStream;
    :catch_13
    move-exception v8

    .line 474
    .local v8, "e":Ljava/lang/Exception;
    :try_start_20
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V
    :try_end_20
    .catch Ljava/lang/Exception; {:try_start_20 .. :try_end_20} :catch_14

    goto/16 :goto_a

    .line 487
    .end local v4    # "bt":[B
    .end local v8    # "e":Ljava/lang/Exception;
    .end local v15    # "file3":Ljava/io/File;
    .end local v25    # "info":Landroid/content/pm/PackageInfo;
    .end local v28    # "manager":Landroid/content/pm/PackageManager;
    .end local v32    # "ver":Ljava/lang/String;
    .end local v33    # "version":Ljava/lang/String;
    :catch_14
    move-exception v8

    .line 488
    .restart local v8    # "e":Ljava/lang/Exception;
    invoke-virtual {v8}, Ljava/lang/Exception;->printStackTrace()V

    .line 489
    sget-object v34, Ljava/lang/System;->out:Ljava/io/PrintStream;

    const-string v35, "get version error"

    invoke-virtual/range {v34 .. v35}, Ljava/io/PrintStream;->println(Ljava/lang/String;)V

    goto/16 :goto_a

    .line 478
    .end local v8    # "e":Ljava/lang/Exception;
    .restart local v4    # "bt":[B
    .restart local v15    # "file3":Ljava/io/File;
    .restart local v25    # "info":Landroid/content/pm/PackageInfo;
    .restart local v28    # "manager":Landroid/content/pm/PackageManager;
    .restart local v33    # "version":Ljava/lang/String;
    :cond_10
    :try_start_21
    new-instance v24, Ljava/io/FileOutputStream;

    .line 479
    invoke-virtual {v15}, Ljava/io/File;->getAbsolutePath()Ljava/lang/String;

    move-result-object v34

    const/16 v35, 0x1

    move-object/from16 v0, v24

    move-object/from16 v1, v34

    move/from16 v2, v35

    invoke-direct {v0, v1, v2}, Ljava/io/FileOutputStream;-><init>(Ljava/lang/String;Z)V
    :try_end_21
    .catch Ljava/lang/Exception; {:try_start_21 .. :try_end_21} :catch_14

    .line 481
    .restart local v24    # "in":Ljava/io/FileOutputStream;
    const/16 v34, 0x0

    :try_start_22
    array-length v0, v4

    move/from16 v35, v0

    move-object/from16 v0, v24

    move/from16 v1, v34

    move/from16 v2, v35

    invoke-virtual {v0, v4, v1, v2}, Ljava/io/FileOutputStream;->write([BII)V

    .line 482
    invoke-virtual/range {v24 .. v24}, Ljava/io/FileOutputStream;->close()V
    :try_end_22
    .catch Ljava/io/IOException; {:try_start_22 .. :try_end_22} :catch_15
    .catch Ljava/lang/Exception; {:try_start_22 .. :try_end_22} :catch_14

    goto/16 :goto_a

    .line 483
    :catch_15
    move-exception v8

    .line 484
    .local v8, "e":Ljava/io/IOException;
    :try_start_23
    invoke-virtual {v8}, Ljava/io/IOException;->printStackTrace()V
    :try_end_23
    .catch Ljava/lang/Exception; {:try_start_23 .. :try_end_23} :catch_14

    goto/16 :goto_a
.end method
