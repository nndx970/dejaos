import { Database } from 'tjs:sqlite';

class AccessControlDB {
    constructor(dbPath = null) {
        this.dbPath = dbPath;
        this.db = null;
        this._initialized = false;
    }

    /**
     * 确保数据库目录存在
     * @returns {Promise<boolean>} 是否成功
     */
    async ensureDbDirectory() {
        try {
            const dirPath = "/data/db";
            // 尝试创建目录（如果不存在）
            await tjs.mkdir(dirPath, { recursive: true });
            return true;
        } catch (error) {
            console.error("创建数据库目录失败:", error);
            return false;
        }
    }

    /**
     * 异步工厂方法 - 推荐使用这种方式创建实例
     */
    static async create(dbPath = '/data/db/access.db') {
        // 先确保数据库目录存在
        const instance = new AccessControlDB();
        await instance.ensureDbDirectory();

        // 然后设置数据库路径并创建连接
        instance.dbPath = dbPath;
        instance.db = new Database(dbPath);

        await instance.init();
        return instance;
    }



    /**
     * 初始化数据库，创建表结构
     */
    async init() {
        if (this._initialized) {
            return;
        }

        // 确保数据库连接已建立
        if (!this.db) {
            throw new Error('数据库连接未建立，请使用 AccessControlDB.create() 方法创建实例');
        }

        // 检查数据库文件是否存在
        let st;
        try {
            st = await tjs.stat(this.dbPath);
        } catch (e) {
        }

        const dbExists = st && st.isFile;

        if (dbExists) {
            console.log(`数据库文件已存在: ${this.dbPath}，检查表结构完整性`);
            this.checkAndCreateTables();
        } else {
            console.log(`数据库文件不存在: ${this.dbPath}，开始创建表结构`);
            this.createTables();
            console.log('数据库表结构创建完成');
        }

        this._initialized = true;
    }

    /**
     * 确保数据库已初始化
     */
    async ensureInitialized() {
        if (!this._initialized) {
            await this.init();
        }
    }

    /**
     * 检查表结构是否完整，如果缺少表则创建
     */
    checkAndCreateTables() {
        const requiredTables = ['ac_user', 'ac_credential', 'ac_permission', 'ac_access_record'];
        const existingTables = this.getExistingTables();

        const missingTables = requiredTables.filter(table => !existingTables.includes(table));

        if (missingTables.length > 0) {
            console.log(`发现缺失的表: ${missingTables.join(', ')}，开始创建`);
            this.createTables();
        } else {
            console.log('所有必需的表都已存在，数据库结构完整');
        }
    }

    /**
     * 获取数据库中已存在的表
     */
    getExistingTables() {
        try {
            const stmt = this.db.prepare(`
                SELECT name FROM sqlite_master 
                WHERE type='table' AND name NOT LIKE 'sqlite_%'
            `);
            const tables = stmt.all();
            return tables.map(table => table.name);
        } catch (error) {
            console.error('获取表列表失败:', error);
            return [];
        }
    }

    /**
     * 创建所有表结构
     */
    createTables() {
        // 创建用户表
        this.db.exec(`
            CREATE TABLE IF NOT EXISTS ac_user (
                id TEXT PRIMARY KEY,                    -- 用户唯一标识符，32位十六进制字符串
                name TEXT NOT NULL,                     -- 用户姓名，必填字段
                phone TEXT,                             -- 用户手机号码
                email TEXT,                             -- 用户邮箱地址
                department TEXT,                        -- 用户所属部门
                position TEXT,                          -- 用户职位/岗位
                status INTEGER DEFAULT 1,               -- 用户状态：1-正常，0-禁用，-1-删除
                extra TEXT,                             -- 扩展信息，JSON格式存储额外用户属性（idCard:身份证号,type:人员类型）
                created_at INTEGER DEFAULT (strftime('%s', 'now')),  -- 创建时间戳（Unix时间戳）
                updated_at INTEGER DEFAULT (strftime('%s', 'now'))   -- 最后更新时间戳（Unix时间戳）
            )
        `);

        // 创建凭证表（支持多种凭证类型）
        this.db.exec(`
            CREATE TABLE IF NOT EXISTS ac_credential (
                id TEXT PRIMARY KEY,                    -- 凭证唯一标识符，32位十六进制字符串
                userId TEXT NOT NULL,                   -- 关联用户ID，外键引用ac_user.id
                type INTEGER NOT NULL,                  -- 凭证类型：100-透传码，101-静态码，103-动态码，200-卡片，300-人脸，400-密码，600-蓝牙，800-按钮
                code TEXT NOT NULL,                     -- 凭证值/编码，如卡号、密码、人脸特征等
                name TEXT,                              -- 凭证名称/描述，便于识别和管理
                status INTEGER DEFAULT 1,               -- 凭证状态：1-正常，0-禁用，-1-删除
                expires_at INTEGER,                     -- 凭证过期时间戳（Unix时间戳），NULL表示永不过期
                extra TEXT,                             -- 扩展信息，JSON格式存储凭证相关额外属性
                created_at INTEGER DEFAULT (strftime('%s', 'now')),  -- 创建时间戳（Unix时间戳）
                updated_at INTEGER DEFAULT (strftime('%s', 'now')),  -- 最后更新时间戳（Unix时间戳）
                FOREIGN KEY (userId) REFERENCES ac_user(id) ON DELETE CASCADE  -- 外键约束，用户删除时级联删除凭证
            )
        `);

        // 创建权限表
        this.db.exec(`
            CREATE TABLE IF NOT EXISTS ac_permission (
                id TEXT PRIMARY KEY,                    -- 权限唯一标识符，32位十六进制字符串
                userId TEXT NOT NULL,                   -- 关联用户ID，外键引用ac_user.id
                door INTEGER NOT NULL,                  -- 门禁设备编号/门号，标识具体的门禁点
                timeType INTEGER NOT NULL,              -- 时间类型：1-一次性，2-重复性，3-永久
                beginTime INTEGER NOT NULL,             -- 权限开始时间戳（Unix时间戳）
                endTime INTEGER NOT NULL,               -- 权限结束时间戳（Unix时间戳）
                repeatBeginTime INTEGER,                -- 重复权限每日开始时间（HHMM格式，如0830表示8:30）
                repeatEndTime INTEGER,                  -- 重复权限每日结束时间（HHMM格式，如1800表示18:00）
                period TEXT,                            -- 重复周期，如"1,2,3,4,5"表示周一到周五，"1,3,5"表示周一三五
                status INTEGER DEFAULT 1,               -- 权限状态：1-正常，0-禁用，-1-删除
                extra TEXT,                             -- 扩展信息，JSON格式存储权限相关额外属性
                created_at INTEGER DEFAULT (strftime('%s', 'now')),  -- 创建时间戳（Unix时间戳）
                updated_at INTEGER DEFAULT (strftime('%s', 'now')),  -- 最后更新时间戳（Unix时间戳）
                FOREIGN KEY (userId) REFERENCES ac_user(id) ON DELETE CASCADE  -- 外键约束，用户删除时级联删除权限
            )
        `);

        // 创建通行记录表
        this.db.exec(`
            CREATE TABLE IF NOT EXISTS ac_access_record (
                id TEXT PRIMARY KEY,                    -- 通行记录唯一标识符，32位十六进制字符串
                credentialId TEXT,                      -- 关联凭证ID，外键引用ac_credential.id，可为NULL（如按钮开门）
                permissionId TEXT,                      -- 关联权限ID，外键引用ac_permission.id，可为NULL（如紧急开门）
                userId TEXT NOT NULL,                   -- 关联用户ID，外键引用ac_user.id
                door INTEGER NOT NULL,                  -- 门禁设备编号/门号，标识具体的门禁点
                accessTime INTEGER NOT NULL,            -- 通行时间戳（Unix时间戳）
                result INTEGER NOT NULL,                -- 通行结果：1-成功，0-失败，-1-拒绝
                method TEXT,                            -- 通行方式，如"card"、"password"、"face"、"button"等
                extra TEXT,                             -- 扩展信息，JSON格式存储通行相关额外属性
                message TEXT,                           -- 通行结果消息，如失败原因、备注信息等
                created_at INTEGER DEFAULT (strftime('%s', 'now')),  -- 记录创建时间戳（Unix时间戳）
                FOREIGN KEY (credentialId) REFERENCES ac_credential(id) ON DELETE SET NULL,  -- 外键约束，凭证删除时设为NULL
                FOREIGN KEY (permissionId) REFERENCES ac_permission(id) ON DELETE SET NULL,  -- 外键约束，权限删除时设为NULL
                FOREIGN KEY (userId) REFERENCES ac_user(id) ON DELETE CASCADE  -- 外键约束，用户删除时级联删除记录
            )
        `);

        // 创建索引以提高查询性能
        this.createIndexes();
    }

    /**
     * 创建索引
     */
    createIndexes() {
        const indexes = [
            'CREATE INDEX IF NOT EXISTS idx_ac_credential_userId ON ac_credential(userId)',
            'CREATE INDEX IF NOT EXISTS idx_ac_credential_type ON ac_credential(type)',
            'CREATE INDEX IF NOT EXISTS idx_ac_credential_code ON ac_credential(code)',
            'CREATE INDEX IF NOT EXISTS idx_ac_credential_status ON ac_credential(status)',
            'CREATE INDEX IF NOT EXISTS idx_ac_permission_userId ON ac_permission(userId)',
            'CREATE INDEX IF NOT EXISTS idx_ac_permission_door ON ac_permission(door)',
            'CREATE INDEX IF NOT EXISTS idx_ac_permission_status ON ac_permission(status)',
            'CREATE INDEX IF NOT EXISTS idx_ac_access_record_userId ON ac_access_record(userId)',
            'CREATE INDEX IF NOT EXISTS idx_ac_access_record_door ON ac_access_record(door)',
            'CREATE INDEX IF NOT EXISTS idx_ac_access_record_time ON ac_access_record(accessTime)',
            'CREATE INDEX IF NOT EXISTS idx_ac_access_record_result ON ac_access_record(result)',
            'CREATE INDEX IF NOT EXISTS idx_ac_user_status ON ac_user(status)',
            'CREATE INDEX IF NOT EXISTS idx_ac_user_department ON ac_user(department)'
        ];

        indexes.forEach(indexSql => {
            this.db.exec(indexSql);
        });
    }

    /**
     * 凭证类型常量
     */
    static get CREDENTIAL_TYPES() {
        return {
            PASSWORD: 400,      // 密码
            CARD: 200,          // 卡片
            FACE: 300,          // 人脸
            QRCODE: 100,        // 100：透传码
            QRCODE_STATIC: 101,// 101：静态码
            QRCODE_DYNAMIC: 103,// 103：动态码
            BLUETOOTH: 600,      // 蓝牙
            BUTTON: 800,      // 按钮
        };
    }

    /**
     * 生成32位随机ID
     */
    generateId() {
        const chars = '0123456789abcdef';
        let result = '';
        for (let i = 0; i < 32; i++) {
            result += chars[Math.floor(Math.random() * chars.length)];
        }
        return result;
    }

    /**
     * 检查ID是否已存在于指定表中
     * @param {string} tableName - 表名
     * @param {string} id - 要检查的ID
     * @returns {boolean} 如果ID已存在返回true，否则返回false
     */
    isIdExists(tableName, id) {
        try {
            const stmt = this.db.prepare(`SELECT COUNT(*) as count FROM ${tableName} WHERE id = ?`);
            const results = stmt.all(id);
            const result = results.length > 0 ? results[0] : null;
            return result && result.count > 0;
        } catch (error) {
            console.error(`检查ID是否存在失败 (表: ${tableName}, ID: ${id}):`, error);
            return false;
        }
    }

    /**
     * 验证并处理ID参数
     * @param {string} tableName - 表名
     * @param {string} providedId - 用户提供的ID（可选）
     * @returns {string} 有效的ID
     * @throws {Error} 如果提供的ID已存在则抛出错误
     */
    validateAndGetId(tableName, providedId = null) {
        if (providedId) {
            // 用户提供了ID，检查是否重复
            if (this.isIdExists(tableName, providedId)) {
                throw new Error(`ID '${providedId}' 在表 '${tableName}' 中已存在`);
            }
            return providedId;
        } else {
            // 用户未提供ID，生成新的唯一ID
            let newId;
            do {
                newId = this.generateId();
            } while (this.isIdExists(tableName, newId));
            return newId;
        }
    }

    // ==================== 用户表操作 ====================

    /**
     * 创建用户
     * @param {string} name - 用户姓名
     * @param {Object} options - 选项对象
     * @param {string} [options.id] - 用户ID，如果不提供则自动生成
     * @param {string} [options.phone] - 手机号码
     * @param {string} [options.email] - 邮箱地址
     * @param {string} [options.department] - 部门
     * @param {string} [options.position] - 职位
     * @param {number} [options.status=1] - 状态：1-正常，0-禁用，-1-删除
     * @param {Object} [options.extra] - 扩展信息
     * @returns {string} 用户ID
     * @throws {Error} 如果提供的ID已存在则抛出错误
     */
    createUser(name, options = {}) {
        const {
            id = null,
            phone = null,
            email = null,
            department = null,
            position = null,
            status = 1,
            extra = null
        } = options;

        // 验证并获取有效的ID
        const userId = this.validateAndGetId('ac_user', id);

        const stmt = this.db.prepare(`
            INSERT INTO ac_user (id, name, phone, email, department, position, status, extra) 
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        `);
        stmt.run(userId, name, phone, email, department, position, status, extra ? JSON.stringify(extra) : null);
        return userId;
    }

    /**
     * 根据ID获取用户
     */
    getUserById(id) {
        const stmt = this.db.prepare(`
            SELECT id, name, phone, email, department, position, status, extra, created_at, updated_at 
            FROM ac_user WHERE id = ?
        `);
        const results = stmt.all(id);
        const result = results.length > 0 ? results[0] : null;
        if (result && result.extra) {
            result.extra = JSON.parse(result.extra);
        }
        return result;
    }

    /**
     * 根据姓名获取用户
     */
    getUserByName(name) {
        const stmt = this.db.prepare(`
            SELECT id, name, extra FROM ac_user WHERE name = ?
        `);
        const results = stmt.all(name);
        const result = results.length > 0 ? results[0] : null;
        if (result && result.extra) {
            result.extra = JSON.parse(result.extra);
        }
        return result;
    }

    /**
     * 获取所有用户
     */
    getAllUsers() {
        const stmt = this.db.prepare(`
            SELECT id, name, extra FROM ac_user ORDER BY name
        `);
        const results = stmt.all();
        return results.map(user => {
            if (user.extra) {
                user.extra = JSON.parse(user.extra);
            }
            return user;
        });
    }

    /**
     * 更新用户
     */
    updateUser(id, name, extra = null) {
        const stmt = this.db.prepare(`
            UPDATE ac_user SET name = ?, extra = ? WHERE id = ?
        `);
        try {
            stmt.run(name, extra ? JSON.stringify(extra) : null, id);
            return true;
        } catch (error) {
            console.error('更新用户失败:', error);
            return false;
        }
    }

    /**
     * 删除用户
     */
    deleteUser(id) {
        const stmt = this.db.prepare(`
            DELETE FROM ac_user WHERE id = ?
        `);
        try {
            stmt.run(id);
            return true;
        } catch (error) {
            console.error('删除用户失败:', error);
            return false;
        }
    }

    // ==================== 凭证表操作 ====================

    /**
     * 创建凭证
     * @param {string} userId - 用户ID
     * @param {number} type - 凭证类型
     * @param {string} code - 凭证值/编码
     * @param {Object} options - 选项对象
     * @param {string} [options.id] - 凭证ID，如果不提供则自动生成
     * @param {string} [options.name] - 凭证名称/描述
     * @param {number} [options.status=1] - 状态：1-正常，0-禁用，-1-删除
     * @param {number} [options.expires_at] - 过期时间戳，NULL表示永不过期
     * @param {Object} [options.extra] - 扩展信息
     * @returns {string} 凭证ID
     * @throws {Error} 如果提供的ID已存在则抛出错误
     */
    createCredential(userId, type, code, options = {}) {
        const {
            id = null,
            name = null,
            status = 1,
            expires_at = null,
            extra = null
        } = options;

        // 验证并获取有效的ID
        const credentialId = this.validateAndGetId('ac_credential', id);

        const stmt = this.db.prepare(`
            INSERT INTO ac_credential (id, userId, type, code, name, status, expires_at, extra) 
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        `);
        stmt.run(credentialId, userId, type, code, name, status, expires_at, extra ? JSON.stringify(extra) : null);
        return credentialId;
    }

    /**
     * 根据ID获取凭证
     */
    getCredentialById(id) {
        const stmt = this.db.prepare(`
            SELECT id, userId, type, code, extra FROM ac_credential WHERE id = ?
        `);
        const results = stmt.all(id);
        const result = results.length > 0 ? results[0] : null;
        if (result && result.extra) {
            result.extra = JSON.parse(result.extra);
        }
        return result;
    }

    /**
     * 根据用户ID获取所有凭证
     */
    getCredentialsByUserId(userId) {
        const stmt = this.db.prepare(`
            SELECT id, userId, type, code, extra FROM ac_credential WHERE userId = ?
        `);
        const results = stmt.all(userId);
        return results.map(credential => {
            if (credential.extra) {
                credential.extra = JSON.parse(credential.extra);
            }
            return credential;
        });
    }

    /**
     * 根据凭证类型和编码查询凭证记录（仅用于门禁验证）
     * @param {number} type - 凭证类型
     * @param {string} code - 凭证编码
     * @returns {Object|null} 凭证记录
     */
    getCredentialByTypeAndCodeAccess(type, code) {
        try {
            const stmt = this.db.prepare(`
                SELECT id, userId, type, code, name, status, expires_at, extra, created_at, updated_at 
                FROM ac_credential 
                WHERE type = ? AND code = ? AND status = 1
            `);
            const results = stmt.all(type, code);

            if (results.length === 0) {
                return null;
            }

            const credential = results[0];

            // 检查凭证是否过期
            if (credential.expires_at && credential.expires_at < Math.floor(Date.now() / 1000)) {
                return null; // 凭证已过期
            }

            // 解析extra字段
            if (credential.extra) {
                credential.extra = JSON.parse(credential.extra);
            }

            return credential;
        } catch (error) {
            console.error("查询凭证记录失败:", error);
            return null;
        }
    }

    /**
     * 更新凭证
     */
    updateCredential(id, type, code, extra = null) {
        const stmt = this.db.prepare(`
            UPDATE ac_credential SET type = ?, code = ?, extra = ? WHERE id = ?
        `);
        try {
            stmt.run(type, code, extra ? JSON.stringify(extra) : null, id);
            return true;
        } catch (error) {
            console.error('更新凭证失败:', error);
            return false;
        }
    }

    /**
     * 删除凭证
     */
    deleteCredential(id) {
        const stmt = this.db.prepare(`
            DELETE FROM ac_credential WHERE id = ?
        `);
        try {
            stmt.run(id);
            return true;
        } catch (error) {
            console.error('删除凭证失败:', error);
            return false;
        }
    }

    /**
     * 根据用户ID删除所有凭证
     */
    deleteCredentialsByUserId(userId) {
        const stmt = this.db.prepare(`
            DELETE FROM ac_credential WHERE userId = ?
        `);
        try {
            stmt.run(userId);
            return true;
        } catch (error) {
            console.error('删除用户所有凭证失败:', error);
            return false;
        }
    }

    // ==================== 权限表操作 ====================

    /**
     * 创建权限
     * @param {string} userId - 用户ID
     * @param {number} door - 门禁设备编号/门号
     * @param {number} timeType - 时间类型：1-一次性，2-重复性，3-永久
     * @param {number} beginTime - 权限开始时间戳
     * @param {number} endTime - 权限结束时间戳
     * @param {Object} options - 选项对象
     * @param {string} [options.id] - 权限ID，如果不提供则自动生成
     * @param {number} [options.repeatBeginTime] - 重复权限每日开始时间（HHMM格式）
     * @param {number} [options.repeatEndTime] - 重复权限每日结束时间（HHMM格式）
     * @param {string} [options.period] - 重复周期，如"1,2,3,4,5"表示周一到周五
     * @param {number} [options.status=1] - 状态：1-正常，0-禁用，-1-删除
     * @param {Object} [options.extra] - 扩展信息
     * @returns {string} 权限ID
     * @throws {Error} 如果提供的ID已存在则抛出错误
     */
    createPermission(userId, door, timeType, beginTime, endTime, options = {}) {
        const {
            id = null,
            repeatBeginTime = null,
            repeatEndTime = null,
            period = null,
            status = 1,
            extra = null
        } = options;

        // 验证并获取有效的ID
        const permissionId = this.validateAndGetId('ac_permission', id);

        const stmt = this.db.prepare(`
            INSERT INTO ac_permission (id, userId, door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, status, extra) 
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        `);
        stmt.run(permissionId, userId, door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, status, extra ? JSON.stringify(extra) : null);
        return permissionId;
    }

    /**
     * 根据ID获取权限
     */
    getPermissionById(id) {
        const stmt = this.db.prepare(`
            SELECT id, userId, door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, extra 
            FROM ac_permission WHERE id = ?
        `);
        const results = stmt.all(id);
        const result = results.length > 0 ? results[0] : null;
        if (result && result.extra) {
            result.extra = JSON.parse(result.extra);
        }
        return result;
    }

    /**
     * 根据用户ID获取所有权限
     */
    getPermissionsByUserId(userId) {
        const stmt = this.db.prepare(`
            SELECT id, userId, door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, status, extra, created_at, updated_at 
            FROM ac_permission WHERE userId = ? AND status = 1
        `);
        const results = stmt.all(userId);
        return results.map(permission => {
            if (permission.extra) {
                permission.extra = JSON.parse(permission.extra);
            }
            return permission;
        });
    }

    /**
     * 根据门号获取权限
     */
    getPermissionsByDoor(door) {
        const stmt = this.db.prepare(`
            SELECT id, userId, door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, status, extra, created_at, updated_at 
            FROM ac_permission WHERE door = ? AND status = 1
        `);
        const results = stmt.all(door);
        return results.map(permission => {
            if (permission.extra) {
                permission.extra = JSON.parse(permission.extra);
            }
            return permission;
        });
    }

    /**
     * 更新权限
     */
    updatePermission(id, door, timeType, beginTime, endTime, repeatBeginTime = null, repeatEndTime = null, period = null, extra = null) {
        const stmt = this.db.prepare(`
            UPDATE ac_permission 
            SET door = ?, timeType = ?, beginTime = ?, endTime = ?, 
                repeatBeginTime = ?, repeatEndTime = ?, period = ?, extra = ? 
            WHERE id = ?
        `);
        try {
            stmt.run(door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, extra ? JSON.stringify(extra) : null, id);
            return true;
        } catch (error) {
            console.error('更新权限失败:', error);
            return false;
        }
    }

    /**
     * 删除权限
     */
    deletePermission(id) {
        const stmt = this.db.prepare(`
            DELETE FROM ac_permission WHERE id = ?
        `);
        try {
            stmt.run(id);
            return true;
        } catch (error) {
            console.error('删除权限失败:', error);
            return false;
        }
    }

    // ==================== 通行记录表操作 ====================

    /**
     * 创建通行记录
     * @param {string} credentialId - 凭证ID（可为null）
     * @param {string} permissionId - 权限ID（可为null）
     * @param {string} userId - 用户ID
     * @param {number} door - 门禁设备编号/门号
     * @param {number} accessTime - 通行时间戳
     * @param {number} result - 通行结果：1-成功，0-失败，-1-拒绝
     * @param {Object} options - 选项对象
     * @param {string} [options.id] - 记录ID，如果不提供则自动生成
     * @param {string} [options.method] - 通行方式，如"card"、"password"、"face"、"button"等
     * @param {Object} [options.extra] - 扩展信息
     * @param {string} [options.message] - 通行结果消息，如失败原因、备注信息等
     * @returns {string} 通行记录ID
     * @throws {Error} 如果提供的ID已存在则抛出错误
     */
    createPassRecord(credentialId, permissionId, userId, door, accessTime, result, options = {}) {
        const {
            id = null,
            method = null,
            extra = null,
            message = null
        } = options;

        // 验证并获取有效的ID
        const recordId = this.validateAndGetId('ac_access_record', id);

        const stmt = this.db.prepare(`
            INSERT INTO ac_access_record (id, credentialId, permissionId, userId, door, accessTime, result, method, extra, message) 
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        `);
        stmt.run(recordId, credentialId, permissionId, userId, door, accessTime, result, method, extra ? JSON.stringify(extra) : null, message);
        return recordId;
    }

    /**
     * 根据ID获取通行记录
     */
    getPassRecordById(id) {
        const stmt = this.db.prepare(`
            SELECT id, credentialId, permissionId, userId, door, accessTime, result, method, extra, message 
            FROM ac_access_record WHERE id = ?
        `);
        const results = stmt.all(id);
        const result = results.length > 0 ? results[0] : null;
        if (result && result.extra) {
            result.extra = JSON.parse(result.extra);
        }
        return result;
    }

    /**
     * 根据用户ID获取通行记录
     */
    getPassRecordsByUserId(userId, limit = 100, offset = 0) {
        const stmt = this.db.prepare(`
            SELECT id, credentialId, permissionId, userId, door, accessTime, result, method, extra, message 
            FROM ac_access_record WHERE userId = ? 
            ORDER BY accessTime DESC LIMIT ? OFFSET ?
        `);
        const results = stmt.all(userId, limit, offset);
        return results.map(record => {
            if (record.extra) {
                record.extra = JSON.parse(record.extra);
            }
            return record;
        });
    }

    /**
     * 根据门号获取通行记录
     */
    getPassRecordsByDoor(door, limit = 100, offset = 0) {
        const stmt = this.db.prepare(`
            SELECT id, credentialId, permissionId, userId, door, accessTime, result, method, extra, message 
            FROM ac_access_record WHERE door = ? 
            ORDER BY accessTime DESC LIMIT ? OFFSET ?
        `);
        const results = stmt.all(door, limit, offset);
        return results.map(record => {
            if (record.extra) {
                record.extra = JSON.parse(record.extra);
            }
            return record;
        });
    }

    /**
     * 根据时间范围获取通行记录
     */
    getPassRecordsByTimeRange(beginTime, endTime, limit = 100, offset = 0) {
        const stmt = this.db.prepare(`
            SELECT id, credentialId, permissionId, userId, door, accessTime, result, method, extra, message 
            FROM ac_access_record WHERE accessTime >= ? AND accessTime <= ? 
            ORDER BY accessTime DESC LIMIT ? OFFSET ?
        `);
        const results = stmt.all(beginTime, endTime, limit, offset);
        return results.map(record => {
            if (record.extra) {
                record.extra = JSON.parse(record.extra);
            }
            return record;
        });
    }

    /**
     * 获取通行记录统计
     */
    getPassRecordStats(userId = null, door = null, beginTime = null, endTime = null) {
        let whereClause = '';
        let params = [];

        if (userId) {
            whereClause += ' WHERE userId = ?';
            params.push(userId);
        }
        if (door) {
            whereClause += whereClause ? ' AND door = ?' : ' WHERE door = ?';
            params.push(door);
        }
        if (beginTime) {
            whereClause += whereClause ? ' AND accessTime >= ?' : ' WHERE accessTime >= ?';
            params.push(beginTime);
        }
        if (endTime) {
            whereClause += whereClause ? ' AND accessTime <= ?' : ' WHERE accessTime <= ?';
            params.push(endTime);
        }

        const stmt = this.db.prepare(`
            SELECT 
                COUNT(*) as total,
                SUM(CASE WHEN result = 1 THEN 1 ELSE 0 END) as success,
                SUM(CASE WHEN result = 0 THEN 1 ELSE 0 END) as failed
            FROM ac_access_record ${whereClause}
        `);
        const results = params.length > 0 ? stmt.all(...params) : stmt.all();
        return results.length > 0 ? results[0] : null;
    }

    /**
     * 更新通行记录
     */
    updatePassRecord(id, result, extra = null, message = null) {
        const stmt = this.db.prepare(`
            UPDATE ac_access_record SET result = ?, extra = ?, message = ? WHERE id = ?
        `);
        try {
            stmt.run(result, extra ? JSON.stringify(extra) : null, message, id);
            return true;
        } catch (error) {
            console.error('更新通行记录失败:', error);
            return false;
        }
    }

    /**
     * 删除通行记录
     */
    deletePassRecord(id) {
        const stmt = this.db.prepare(`
            DELETE FROM ac_access_record WHERE id = ?
        `);
        try {
            stmt.run(id);
            return true;
        } catch (error) {
            console.error('删除凭证失败:', error);
            return false;
        }
    }

    // ==================== 分页查询方法 ====================

    /**
     * 分页查询用户列表
     * @param {Object} options - 查询选项
     * @param {number} options.page - 页码，从1开始
     * @param {number} options.pageSize - 每页大小，默认20
     * @param {string} options.orderBy - 排序字段，默认created_at
     * @param {string} options.order - 排序方向，ASC或DESC，默认DESC
     * @param {string} options.department - 部门筛选
     * @param {number} options.status - 状态筛选
     * @returns {Object} 包含data和pagination信息的对象
     */
    getUsersPaginated(options = {}) {
        const {
            page = 1,
            pageSize = 20,
            orderBy = 'created_at',
            order = 'DESC',
            department = null,
            status = null
        } = options;

        const offset = (page - 1) * pageSize;
        let whereClause = '';
        let params = [];

        if (department) {
            whereClause += ' WHERE department = ?';
            params.push(department);
        }
        if (status !== null) {
            whereClause += whereClause ? ' AND status = ?' : ' WHERE status = ?';
            params.push(status);
        }

        // 查询总数
        const countStmt = this.db.prepare(`
            SELECT COUNT(*) as total FROM ac_user ${whereClause}
        `);
        const totalResult = params.length > 0 ? countStmt.all(...params) : countStmt.all();
        const total = totalResult[0].total;

        // 查询数据
        const dataStmt = this.db.prepare(`
            SELECT id, name, phone, email, department, position, status, extra, created_at, updated_at 
            FROM ac_user ${whereClause}
            ORDER BY ${orderBy} ${order}
            LIMIT ? OFFSET ?
        `);
        const data = params.length > 0 ? dataStmt.all(...params, pageSize, offset) : dataStmt.all(pageSize, offset);

        // 处理extra字段
        const processedData = data.map(user => {
            if (user.extra) {
                user.extra = JSON.parse(user.extra);
            }
            return user;
        });

        return {
            data: processedData,
            pagination: {
                page,
                pageSize,
                total,
                totalPages: Math.ceil(total / pageSize),
                hasNext: page < Math.ceil(total / pageSize),
                hasPrev: page > 1
            }
        };
    }

    /**
     * 根据用户名模糊查询且使用分页
     * @param {Object} options - 查询选项
     * @param {number} options.page - 页码，从1开始
     * @param {number} options.pageSize - 每页大小，默认20
     * @param {string} options.orderBy - 排序字段，默认created_at
     * @param {string} options.order - 排序方向，ASC或DESC，默认DESC
     * @param {string} options.name - 用户名模糊查询关键词
     * @param {string} options.department - 部门筛选
     * @param {number} options.status - 状态筛选
     * @returns {Object} 包含data和pagination信息的对象
     */
    getUsersByNamePaginated(options = {}) {
        const {
            page = 1,
            pageSize = 20,
            orderBy = 'created_at',
            order = 'DESC',
            name = null,
            department = null,
            status = null
        } = options;

        const offset = (page - 1) * pageSize;
        let whereClause = '';
        let params = [];

        // 构建WHERE条件
        const conditions = [];

        if (name) {
            conditions.push('name LIKE ?');
            params.push(`%${name}%`);
        }
        if (department) {
            conditions.push('department = ?');
            params.push(department);
        }
        if (status !== null) {
            conditions.push('status = ?');
            params.push(status);
        }

        if (conditions.length > 0) {
            whereClause = ' WHERE ' + conditions.join(' AND ');
        }

        // 查询总数
        const countStmt = this.db.prepare(`
            SELECT COUNT(*) as total FROM ac_user ${whereClause}
        `);
        const totalResult = params.length > 0 ? countStmt.all(...params) : countStmt.all();
        const total = totalResult[0].total;

        // 查询数据
        const dataStmt = this.db.prepare(`
            SELECT id, name, phone, email, department, position, status, extra, created_at, updated_at 
            FROM ac_user ${whereClause}
            ORDER BY ${orderBy} ${order}
            LIMIT ? OFFSET ?
        `);
        const data = params.length > 0 ? dataStmt.all(...params, pageSize, offset) : dataStmt.all(pageSize, offset);

        // 处理extra字段
        const processedData = data.map(user => {
            if (user.extra) {
                user.extra = JSON.parse(user.extra);
            }
            return user;
        });

        return {
            data: processedData,
            pagination: {
                page,
                pageSize,
                total,
                totalPages: Math.ceil(total / pageSize),
                hasNext: page < Math.ceil(total / pageSize),
                hasPrev: page > 1
            }
        };
    }

    /**
     * 分页查询凭证列表
     * @param {Object} options - 查询选项
     * @param {number} options.page - 页码，从1开始
     * @param {number} options.pageSize - 每页大小，默认20
     * @param {string} options.orderBy - 排序字段，默认created_at
     * @param {string} options.order - 排序方向，ASC或DESC，默认DESC
     * @param {string} options.userId - 用户ID筛选
     * @param {number} options.type - 凭证类型筛选
     * @param {number} options.status - 状态筛选
     * @returns {Object} 包含data和pagination信息的对象
     */
    getCredentialsPaginated(options = {}) {
        const {
            page = 1,
            pageSize = 20,
            orderBy = 'created_at',
            order = 'DESC',
            userId = null,
            type = null,
            status = null
        } = options;

        const offset = (page - 1) * pageSize;
        let whereClause = '';
        let params = [];

        if (userId) {
            whereClause += ' WHERE userId = ?';
            params.push(userId);
        }
        if (type !== null) {
            whereClause += whereClause ? ' AND type = ?' : ' WHERE type = ?';
            params.push(type);
        }
        if (status !== null) {
            whereClause += whereClause ? ' AND status = ?' : ' WHERE status = ?';
            params.push(status);
        }

        // 查询总数
        const countStmt = this.db.prepare(`
            SELECT COUNT(*) as total FROM ac_credential ${whereClause}
        `);
        const totalResult = params.length > 0 ? countStmt.all(...params) : countStmt.all();
        const total = totalResult[0].total;

        // 查询数据
        const dataStmt = this.db.prepare(`
            SELECT id, userId, type, code, name, status, expires_at, extra, created_at, updated_at 
            FROM ac_credential ${whereClause}
            ORDER BY ${orderBy} ${order}
            LIMIT ? OFFSET ?
        `);
        const data = params.length > 0 ? dataStmt.all(...params, pageSize, offset) : dataStmt.all(pageSize, offset);

        // 处理extra字段
        const processedData = data.map(credential => {
            if (credential.extra) {
                credential.extra = JSON.parse(credential.extra);
            }
            return credential;
        });

        return {
            data: processedData,
            pagination: {
                page,
                pageSize,
                total,
                totalPages: Math.ceil(total / pageSize),
                hasNext: page < Math.ceil(total / pageSize),
                hasPrev: page > 1
            }
        };
    }

    /**
     * 分页查询权限列表
     * @param {Object} options - 查询选项
     * @param {number} options.page - 页码，从1开始
     * @param {number} options.pageSize - 每页大小，默认20
     * @param {string} options.orderBy - 排序字段，默认created_at
     * @param {string} options.order - 排序方向，ASC或DESC，默认DESC
     * @param {string} options.userId - 用户ID筛选
     * @param {number} options.door - 门号筛选
     * @param {number} options.status - 状态筛选
     * @returns {Object} 包含data和pagination信息的对象
     */
    getPermissionsPaginated(options = {}) {
        const {
            page = 1,
            pageSize = 20,
            orderBy = 'created_at',
            order = 'DESC',
            userId = null,
            door = null,
            status = null
        } = options;

        const offset = (page - 1) * pageSize;
        let whereClause = '';
        let params = [];

        if (userId) {
            whereClause += ' WHERE userId = ?';
            params.push(userId);
        }
        if (door !== null) {
            whereClause += whereClause ? ' AND door = ?' : ' WHERE door = ?';
            params.push(door);
        }
        if (status !== null) {
            whereClause += whereClause ? ' AND status = ?' : ' WHERE status = ?';
            params.push(status);
        }

        // 查询总数
        const countStmt = this.db.prepare(`
            SELECT COUNT(*) as total FROM ac_permission ${whereClause}
        `);
        const totalResult = params.length > 0 ? countStmt.all(...params) : countStmt.all();
        const total = totalResult[0].total;

        // 查询数据
        const dataStmt = this.db.prepare(`
            SELECT id, userId, door, timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period, status, extra, created_at, updated_at 
            FROM ac_permission ${whereClause}
            ORDER BY ${orderBy} ${order}
            LIMIT ? OFFSET ?
        `);
        const data = params.length > 0 ? dataStmt.all(...params, pageSize, offset) : dataStmt.all(pageSize, offset);

        // 处理extra字段
        const processedData = data.map(permission => {
            if (permission.extra) {
                permission.extra = JSON.parse(permission.extra);
            }
            return permission;
        });

        return {
            data: processedData,
            pagination: {
                page,
                pageSize,
                total,
                totalPages: Math.ceil(total / pageSize),
                hasNext: page < Math.ceil(total / pageSize),
                hasPrev: page > 1
            }
        };
    }

    /**
     * 分页查询通行记录列表
     * @param {Object} options - 查询选项
     * @param {number} options.page - 页码，从1开始
     * @param {number} options.pageSize - 每页大小，默认20
     * @param {string} options.orderBy - 排序字段，默认accessTime
     * @param {string} options.order - 排序方向，ASC或DESC，默认DESC
     * @param {string} options.userId - 用户ID筛选
     * @param {number} options.door - 门号筛选
     * @param {number} options.result - 通行结果筛选
     * @param {number} options.beginTime - 开始时间筛选
     * @param {number} options.endTime - 结束时间筛选
     * @returns {Object} 包含data和pagination信息的对象
     */
    getAccessRecordsPaginated(options = {}) {
        const {
            page = 1,
            pageSize = 20,
            orderBy = 'accessTime',
            order = 'DESC',
            userId = null,
            door = null,
            result = null,
            beginTime = null,
            endTime = null
        } = options;

        const offset = (page - 1) * pageSize;
        let whereClause = '';
        let params = [];

        if (userId) {
            whereClause += ' WHERE userId = ?';
            params.push(userId);
        }
        if (door !== null) {
            whereClause += whereClause ? ' AND door = ?' : ' WHERE door = ?';
            params.push(door);
        }
        if (result !== null) {
            whereClause += whereClause ? ' AND result = ?' : ' WHERE result = ?';
            params.push(result);
        }
        if (beginTime) {
            whereClause += whereClause ? ' AND accessTime >= ?' : ' WHERE accessTime >= ?';
            params.push(beginTime);
        }
        if (endTime) {
            whereClause += whereClause ? ' AND accessTime <= ?' : ' WHERE accessTime <= ?';
            params.push(endTime);
        }

        // 查询总数
        const countStmt = this.db.prepare(`
            SELECT COUNT(*) as total FROM ac_access_record ${whereClause}
        `);
        const totalResult = params.length > 0 ? countStmt.all(...params) : countStmt.all();
        const total = totalResult[0].total;

        // 查询数据
        const dataStmt = this.db.prepare(`
            SELECT id, credentialId, permissionId, userId, door, accessTime, result, method, extra, message, created_at 
            FROM ac_access_record ${whereClause}
            ORDER BY ${orderBy} ${order}
            LIMIT ? OFFSET ?
        `);
        const data = params.length > 0 ? dataStmt.all(...params, pageSize, offset) : dataStmt.all(pageSize, offset);

        // 处理extra字段
        const processedData = data.map(record => {
            if (record.extra) {
                record.extra = JSON.parse(record.extra);
            }
            return record;
        });

        return {
            data: processedData,
            pagination: {
                page,
                pageSize,
                total,
                totalPages: Math.ceil(total / pageSize),
                hasNext: page < Math.ceil(total / pageSize),
                hasPrev: page > 1
            }
        };
    }

    // ==================== 综合查询方法 ====================

    /**
     * 获取用户完整信息（包含凭证和权限）
     */
    getUserFullInfo(userId) {
        const user = this.getUserById(userId);
        if (!user) return null;

        const credentials = this.getCredentialsByUserId(userId);
        const permissions = this.getPermissionsByUserId(userId);

        return {
            ...user,
            credentials,
            permissions
        };
    }


    /**
     * 获取数据库状态信息
     */
    getDatabaseStatus() {
        const dbExists = existsSync(this.dbPath);
        const existingTables = this.getExistingTables();
        const requiredTables = ['ac_user', 'ac_credential', 'ac_permission', 'ac_access_record'];
        const missingTables = requiredTables.filter(table => !existingTables.includes(table));

        return {
            dbPath: this.dbPath,
            dbExists: dbExists,
            existingTables: existingTables,
            requiredTables: requiredTables,
            missingTables: missingTables,
            isComplete: missingTables.length === 0
        };
    }

    /**
     * 强制重新创建所有表（慎用！会删除所有数据）
     */
    recreateTables() {
        console.warn('警告：正在重新创建所有表，这将删除所有现有数据！');

        // 删除所有表
        const existingTables = this.getExistingTables();
        existingTables.forEach(table => {
            this.db.exec(`DROP TABLE IF EXISTS ${table}`);
        });

        // 重新创建表
        this.createTables();
        console.log('所有表已重新创建');
    }

    /**
     * 根据权限记录构建时间配置对象（用于门禁验证）
     * @param {Object} permission - 权限记录
     * @returns {Object} 时间配置对象
     */
    buildTimeConfig(permission) {
        const { timeType, beginTime, endTime, repeatBeginTime, repeatEndTime, period } = permission;

        switch (timeType) {
            case 0:
                // 永久权限
                return {
                    type: 0 // 永久权限
                };
            case 1:
                // 一次性权限
                return {
                    type: 1,
                    range: {
                        beginTime: beginTime,
                        endTime: endTime
                    }
                };
            case 2:
                // 重复性权限
                return {
                    type: 2,
                    range: {
                        beginTime: beginTime,
                        endTime: endTime
                    },
                    beginTime: repeatBeginTime,
                    endTime: repeatEndTime
                };
            case 3:
                // 周重复权限
                return {
                    type: 3,
                    range: {
                        beginTime: beginTime,
                        endTime: endTime
                    },
                    beginTime: repeatBeginTime,
                    endTime: repeatEndTime,
                    weekPeriodTime: period
                };
        }
    }

    /**
     * 关闭数据库连接
     */
    close() {
        this.db.close();
    }
}

// 导出数据库实例
let db;

try {
    db = await AccessControlDB.create();
    console.log('数据库实例创建成功');
} catch (error) {
    console.error('创建数据库实例失败:', error);
    // 创建一个空的实例作为fallback
    db = new AccessControlDB();
    console.log('使用fallback数据库实例');
}
export default db;
