namespace Rynex
{
    public static class AssetManager
    {
#if false
        public static T GetAsset<T>(string path) where T : Asset, new()
        {
            T classType;
            InternalCalls.AssetManger_GetAsset_Path(path, classType, out ulong handle);
            T asset = CreateAssetInstance<T>(handle);
            return asset;
        }

        public static T GetAsset<T>(ulong handle) where T : Asset, new()
        {
            Type classType = typeof(T);
            InternalCalls.AssetManger_GetAsset_Handle(handle, classType);
            T asset = CreateAssetInstance<T>(handle);
            
            return asset;
        }
        private static T CreateAssetInstance<T>(ulong handle) where T : Asset
        {
            // Verwenden Sie Reflection, um den Konstruktor mit dem handle-Parameter zu finden
            var constructor = typeof(T).GetConstructor(
                BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic,
                null,
                new[] { typeof(ulong) },
                null);

            if (constructor == null)
            {
                throw new InvalidOperationException($"Kein geeigneter Konstruktor für {typeof(T).Name} gefunden.");
            }

            // Erstellen Sie die Instanz mit dem gefundenen Konstruktor
            return (T)constructor.Invoke(new object[] { handle });
        }

#endif
    }
}
