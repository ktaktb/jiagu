.class public Lcom/example/forshell/Smith;
.super Ljava/lang/Object;
.source "Smith.java"


# annotations
.annotation system Ldalvik/annotation/Signature;
    value = {
        "<T:",
        "Ljava/lang/Object;",
        ">",
        "Ljava/lang/Object;"
    }
.end annotation


# instance fields
.field private field:Ljava/lang/reflect/Field;

.field private fieldName:Ljava/lang/String;

.field private inited:Z

.field private obj:Ljava/lang/Object;


# direct methods
.method public constructor <init>(Ljava/lang/Object;Ljava/lang/String;)V
    .locals 2
    .param p1, "obj"    # Ljava/lang/Object;
    .param p2, "fieldName"    # Ljava/lang/String;

    .prologue
    .line 12
    .local p0, "this":Lcom/example/forshell/Smith;, "Lcom/example/forshell/Smith<TT;>;"
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    .line 13
    if-nez p1, :cond_0

    .line 14
    new-instance v0, Ljava/lang/IllegalArgumentException;

    const-string v1, "obj cannot be null"

    invoke-direct {v0, v1}, Ljava/lang/IllegalArgumentException;-><init>(Ljava/lang/String;)V

    throw v0

    .line 16
    :cond_0
    iput-object p1, p0, Lcom/example/forshell/Smith;->obj:Ljava/lang/Object;

    .line 17
    iput-object p2, p0, Lcom/example/forshell/Smith;->fieldName:Ljava/lang/String;

    .line 18
    return-void
.end method

.method public static getField(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/reflect/Field;
    .locals 5
    .param p1, "name"    # Ljava/lang/String;
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "(",
            "Ljava/lang/Class",
            "<*>;",
            "Ljava/lang/String;",
            ")",
            "Ljava/lang/reflect/Field;"
        }
    .end annotation

    .prologue
    .line 72
    .local p0, "cls":Ljava/lang/Class;, "Ljava/lang/Class<*>;"
    invoke-virtual {p0}, Ljava/lang/Class;->getDeclaredFields()[Ljava/lang/reflect/Field;

    move-result-object v2

    array-length v3, v2

    const/4 v1, 0x0

    :goto_0
    if-ge v1, v3, :cond_2

    aget-object v0, v2, v1

    .line 73
    .local v0, "field":Ljava/lang/reflect/Field;
    invoke-virtual {v0}, Ljava/lang/reflect/Field;->isAccessible()Z

    move-result v4

    if-nez v4, :cond_0

    .line 74
    const/4 v4, 0x1

    invoke-virtual {v0, v4}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 76
    :cond_0
    invoke-virtual {v0}, Ljava/lang/reflect/Field;->getName()Ljava/lang/String;

    move-result-object v4

    invoke-virtual {v4, p1}, Ljava/lang/String;->equals(Ljava/lang/Object;)Z

    move-result v4

    if-eqz v4, :cond_1

    .line 80
    .end local v0    # "field":Ljava/lang/reflect/Field;
    :goto_1
    return-object v0

    .line 72
    .restart local v0    # "field":Ljava/lang/reflect/Field;
    :cond_1
    add-int/lit8 v1, v1, 0x1

    goto :goto_0

    .line 80
    .end local v0    # "field":Ljava/lang/reflect/Field;
    :cond_2
    const/4 v0, 0x0

    goto :goto_1
.end method

.method private prepare()V
    .locals 4

    .prologue
    .local p0, "this":Lcom/example/forshell/Smith;, "Lcom/example/forshell/Smith<TT;>;"
    const/4 v3, 0x1

    .line 21
    iget-boolean v2, p0, Lcom/example/forshell/Smith;->inited:Z

    if-eqz v2, :cond_1

    .line 40
    :cond_0
    :goto_0
    return-void

    .line 23
    :cond_1
    iput-boolean v3, p0, Lcom/example/forshell/Smith;->inited:Z

    .line 25
    iget-object v2, p0, Lcom/example/forshell/Smith;->obj:Ljava/lang/Object;

    invoke-virtual {v2}, Ljava/lang/Object;->getClass()Ljava/lang/Class;

    move-result-object v0

    .line 26
    .local v0, "c":Ljava/lang/Class;, "Ljava/lang/Class<*>;"
    :goto_1
    if-eqz v0, :cond_0

    .line 28
    :try_start_0
    iget-object v2, p0, Lcom/example/forshell/Smith;->fieldName:Ljava/lang/String;

    invoke-virtual {v0, v2}, Ljava/lang/Class;->getDeclaredField(Ljava/lang/String;)Ljava/lang/reflect/Field;

    move-result-object v1

    .line 32
    .local v1, "f":Ljava/lang/reflect/Field;
    const/4 v2, 0x1

    invoke-virtual {v1, v2}, Ljava/lang/reflect/Field;->setAccessible(Z)V

    .line 33
    iput-object v1, p0, Lcom/example/forshell/Smith;->field:Ljava/lang/reflect/Field;
    :try_end_0
    .catch Ljava/lang/Exception; {:try_start_0 .. :try_end_0} :catch_0
    .catchall {:try_start_0 .. :try_end_0} :catchall_0

    .line 37
    invoke-virtual {v0}, Ljava/lang/Class;->getSuperclass()Ljava/lang/Class;

    move-result-object v0

    .line 34
    goto :goto_0

    .line 35
    .end local v1    # "f":Ljava/lang/reflect/Field;
    :catch_0
    move-exception v2

    .line 37
    invoke-virtual {v0}, Ljava/lang/Class;->getSuperclass()Ljava/lang/Class;

    move-result-object v0

    .line 38
    goto :goto_1

    .line 37
    :catchall_0
    move-exception v2

    invoke-virtual {v0}, Ljava/lang/Class;->getSuperclass()Ljava/lang/Class;

    move-result-object v0

    throw v2
.end method


# virtual methods
.method public get()Ljava/lang/Object;
    .locals 4
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "()TT;"
        }
    .end annotation

    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/lang/NoSuchFieldException;,
            Ljava/lang/IllegalAccessException;,
            Ljava/lang/IllegalArgumentException;
        }
    .end annotation

    .prologue
    .line 44
    .local p0, "this":Lcom/example/forshell/Smith;, "Lcom/example/forshell/Smith<TT;>;"
    invoke-direct {p0}, Lcom/example/forshell/Smith;->prepare()V

    .line 46
    iget-object v2, p0, Lcom/example/forshell/Smith;->field:Ljava/lang/reflect/Field;

    if-nez v2, :cond_0

    .line 47
    new-instance v2, Ljava/lang/NoSuchFieldException;

    invoke-direct {v2}, Ljava/lang/NoSuchFieldException;-><init>()V

    throw v2

    .line 51
    :cond_0
    :try_start_0
    iget-object v2, p0, Lcom/example/forshell/Smith;->field:Ljava/lang/reflect/Field;

    iget-object v3, p0, Lcom/example/forshell/Smith;->obj:Ljava/lang/Object;

    invoke-virtual {v2, v3}, Ljava/lang/reflect/Field;->get(Ljava/lang/Object;)Ljava/lang/Object;
    :try_end_0
    .catch Ljava/lang/ClassCastException; {:try_start_0 .. :try_end_0} :catch_0

    move-result-object v1

    .line 53
    .local v1, "r":Ljava/lang/Object;, "TT;"
    return-object v1

    .line 54
    .end local v1    # "r":Ljava/lang/Object;, "TT;"
    :catch_0
    move-exception v0

    .line 55
    .local v0, "e":Ljava/lang/ClassCastException;
    new-instance v2, Ljava/lang/IllegalArgumentException;

    const-string v3, "unable to cast object"

    invoke-direct {v2, v3}, Ljava/lang/IllegalArgumentException;-><init>(Ljava/lang/String;)V

    throw v2
.end method

.method public set(Ljava/lang/Object;)V
    .locals 2
    .annotation system Ldalvik/annotation/Signature;
        value = {
            "(TT;)V"
        }
    .end annotation

    .annotation system Ldalvik/annotation/Throws;
        value = {
            Ljava/lang/NoSuchFieldException;,
            Ljava/lang/IllegalAccessException;,
            Ljava/lang/IllegalArgumentException;
        }
    .end annotation

    .prologue
    .line 61
    .local p0, "this":Lcom/example/forshell/Smith;, "Lcom/example/forshell/Smith<TT;>;"
    .local p1, "val":Ljava/lang/Object;, "TT;"
    invoke-direct {p0}, Lcom/example/forshell/Smith;->prepare()V

    .line 63
    iget-object v0, p0, Lcom/example/forshell/Smith;->field:Ljava/lang/reflect/Field;

    if-nez v0, :cond_0

    .line 64
    new-instance v0, Ljava/lang/NoSuchFieldException;

    invoke-direct {v0}, Ljava/lang/NoSuchFieldException;-><init>()V

    throw v0

    .line 66
    :cond_0
    iget-object v0, p0, Lcom/example/forshell/Smith;->field:Ljava/lang/reflect/Field;

    iget-object v1, p0, Lcom/example/forshell/Smith;->obj:Ljava/lang/Object;

    invoke-virtual {v0, v1, p1}, Ljava/lang/reflect/Field;->set(Ljava/lang/Object;Ljava/lang/Object;)V

    .line 68
    return-void
.end method
