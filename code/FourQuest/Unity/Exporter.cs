using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEditor;
using UnityEngine;
using UnityEngine.Experimental.GlobalIllumination;
using UnityEngine.SceneManagement;
using static UnityEditor.Progress;

[System.Serializable]
public class EngineVector2
{
    public float x;
    public float y;
}

[System.Serializable]
public class EngineVector3
{
    public float x;
    public float y;
    public float z;
}

[System.Serializable]
public class EngineVector4
{
    public float x;
    public float y;
    public float z;
    public float w;
}

[System.Serializable]
public class EngineTransform
{
    public EngineVector3 Position;
    public EngineVector4 Rotation;
    public EngineVector3 Scale;
}

[System.Serializable]
public class EngineMaterial
{
    public string Name;
    public string renderMode;

    public EngineVector4 Albedo;
    public float Metallic;
    public float Smoothness; // 유니티는 Roughness 대신 smoothness
    public EngineVector4 Emission;

    public string AlbedoMap;
    public string MetallicAndSmoothnessMap;
    public string NormalMap;
    public string EmissionMap;

    public EngineVector2 Tiling;
    public EngineVector2 Offset;
}

[System.Serializable]
public class EngineMesh
{
    public string ModelPath;
    public string Name;
    public List<EngineMaterial> Materials;
    public EngineVector4 LightmapScaleOffset;
    public int LightmapIndex;
}

[System.Serializable]
public class EngineLight
{
    public string Type;
    public EngineVector4 Color;
    public float Intensity;
    public float Range;
    public float SpotAngle; // 우리는 그냥 Spot 으로 되어있어서 바꿔야 할지도 
    public string Mode;
}

[System.Serializable]
public class EngineGameObject
{
    public string Name;
    public int ID;
    public int ParentID;
    public EngineTransform Transform;
    public EngineMesh Mesh;
    public EngineLight Light;
    public bool isStatic;
}

[System.Serializable]
public class EngineGameObjects
{
    public List<EngineGameObject> GameObjects;
}

public class Exporter : MonoBehaviour
{
    void PrintGameObject(ref EngineGameObjects objects, GameObject go)
    {
        EngineGameObject engineGameObject = new EngineGameObject();

        {
            // 게임오브젝트 이름 && ID
            engineGameObject.Name = go.name;
            engineGameObject.ID = go.GetInstanceID();
            engineGameObject.ParentID = 0;
            if (go.transform.parent != null)
            {
                engineGameObject.ParentID = go.transform.parent.GetInstanceID();
            }

            engineGameObject.isStatic = go.isStatic;
        }

        // Transform 컴포넌트
        {
            Transform transform = go.GetComponent<Transform>();
            engineGameObject.Transform = new EngineTransform();

            engineGameObject.Transform.Position = new EngineVector3();
            engineGameObject.Transform.Position.x = transform.localPosition.x;
            engineGameObject.Transform.Position.y = transform.localPosition.y;
            engineGameObject.Transform.Position.z = transform.localPosition.z;

            engineGameObject.Transform.Rotation = new EngineVector4();
            engineGameObject.Transform.Rotation.x = transform.localRotation.x;
            engineGameObject.Transform.Rotation.y = transform.localRotation.y;
            engineGameObject.Transform.Rotation.z = transform.localRotation.z;
            engineGameObject.Transform.Rotation.w = transform.localRotation.w;

            engineGameObject.Transform.Scale = new EngineVector3();
            engineGameObject.Transform.Scale.x = transform.localScale.x;
            engineGameObject.Transform.Scale.y = transform.localScale.y;
            engineGameObject.Transform.Scale.z = transform.localScale.z;
        }

        // MeshRenderer 컴포넌트
        MeshFilter meshFilter = go.GetComponent<MeshFilter>();
        if (meshFilter != null)
        {
            engineGameObject.Mesh = new EngineMesh();
            engineGameObject.Mesh.Name = meshFilter.sharedMesh.name;

            string modelPath = AssetDatabase.GetAssetPath(meshFilter.sharedMesh);
            modelPath = ResourcePathSetting(modelPath);
            engineGameObject.Mesh.ModelPath = modelPath;

            MeshRenderer meshRenderer = go.GetComponent<MeshRenderer>();

            if (meshRenderer != null)
            {


                engineGameObject.Mesh.LightmapScaleOffset = new EngineVector4();
                engineGameObject.Mesh.LightmapScaleOffset.x = meshRenderer.lightmapScaleOffset.x;
                engineGameObject.Mesh.LightmapScaleOffset.y = meshRenderer.lightmapScaleOffset.y;
                engineGameObject.Mesh.LightmapScaleOffset.z = meshRenderer.lightmapScaleOffset.z;
                engineGameObject.Mesh.LightmapScaleOffset.w = meshRenderer.lightmapScaleOffset.w;

                engineGameObject.Mesh.LightmapIndex = meshRenderer.lightmapIndex;

                engineGameObject.Mesh.Materials = new List<EngineMaterial>();

                Material[] materials = meshRenderer.materials;

                foreach (Material mat in materials)
                {
                    EngineMaterial newMaterial = new EngineMaterial();

                    // Name
                    newMaterial.Name = mat.name;

                    // RenderMode
                    int renderMode = (int)mat.GetFloat("_Mode");
                    string modeString = "Unknown";
                    if (renderMode == 0) modeString = "Opaque";
                    else if (renderMode == 1) modeString = "Cutout";
                    else if (renderMode == 2) modeString = "Fade";
                    else if (renderMode == 3) modeString = "Transparent";
                    newMaterial.renderMode = modeString;

                    // Albedo
                    Color albedo = mat.GetColor("_Color");
                    newMaterial.Albedo = new EngineVector4();
                    newMaterial.Albedo.x = albedo.r;
                    newMaterial.Albedo.y = albedo.g;
                    newMaterial.Albedo.z = albedo.b;
                    newMaterial.Albedo.w = albedo.a;

                    // Metallic
                    newMaterial.Metallic = mat.GetFloat("_Metallic");

                    // Smoothness
                    newMaterial.Smoothness = mat.GetFloat("_Glossiness");

                    // Emission
                    Color emission = mat.GetColor("_EmissionColor");
                    newMaterial.Emission = new EngineVector4();
                    newMaterial.Emission.x = emission.r;
                    newMaterial.Emission.y = emission.g;
                    newMaterial.Emission.z = emission.b;
                    newMaterial.Emission.w = emission.a;

                    // AlbedoMap
                    Texture albedoMap = mat.GetTexture("_MainTex");
                    string albedoMapString = AssetDatabase.GetAssetPath(albedoMap);
                    albedoMapString = ResourcePathSetting(albedoMapString);
                    newMaterial.AlbedoMap = albedoMapString;

                    // MetallicAndSmoothnessMap
                    Texture metallicAndSmoothnessMap = mat.GetTexture("_MetallicGlossMap");
                    string metallicAndSmoothnessMapString = AssetDatabase.GetAssetPath(metallicAndSmoothnessMap);
                    metallicAndSmoothnessMapString = ResourcePathSetting(metallicAndSmoothnessMapString);
                    newMaterial.MetallicAndSmoothnessMap = metallicAndSmoothnessMapString;

                    // NormalMap
                    Texture normalMap = mat.GetTexture("_BumpMap");
                    string normalMapString = AssetDatabase.GetAssetPath(normalMap);
                    normalMapString = ResourcePathSetting(normalMapString);
                    newMaterial.NormalMap = normalMapString;

                    // EmissionMap
                    Texture emissionMap = mat.GetTexture("_EmissionMap");
                    string emissionMapString = AssetDatabase.GetAssetPath(emissionMap);
                    emissionMapString = ResourcePathSetting(emissionMapString);
                    newMaterial.EmissionMap = emissionMapString;

                    // Tiling
                    Vector2 tiling = mat.GetTextureScale("_MainTex");
                    newMaterial.Tiling = new EngineVector2();
                    newMaterial.Tiling.x = tiling.x;
                    newMaterial.Tiling.y = tiling.y;

                    // Offset
                    Vector2 offset = mat.GetTextureOffset("_MainTex");
                    newMaterial.Offset = new EngineVector2();
                    newMaterial.Offset.x = offset.x;
                    newMaterial.Offset.y = offset.y;

                    engineGameObject.Mesh.Materials.Add(newMaterial);
                }
            }
        }

        // Light 컴포넌트
        Light light = go.GetComponent<Light>();
        if (light != null)
        {
            // LightType
            UnityEngine.LightType lightType = light.type;
            engineGameObject.Light = new EngineLight();
            string lightTypeString = "Unknown";
            if (lightType == UnityEngine.LightType.Spot) lightTypeString = "Spot";
            else if (lightType == UnityEngine.LightType.Directional) lightTypeString = "Directional";
            else if (lightType == UnityEngine.LightType.Point) lightTypeString = "Point";
            engineGameObject.Light.Type = lightTypeString;

            // Color
            Color color = light.color;
            engineGameObject.Light.Color = new EngineVector4();
            engineGameObject.Light.Color.x = color.r;
            engineGameObject.Light.Color.y = color.g;
            engineGameObject.Light.Color.z = color.b;
            engineGameObject.Light.Color.w = color.a;

            // Intensity
            engineGameObject.Light.Intensity = light.intensity;

            // Range
            engineGameObject.Light.Range = light.range;

            // SpotAngle
            engineGameObject.Light.SpotAngle = light.spotAngle;

            LightmapBakeType lightMode = light.lightmapBakeType;
            if (lightMode == LightmapBakeType.Realtime) lightTypeString = "Realtime";
            else if (lightMode == LightmapBakeType.Mixed) lightTypeString = "Mixed";
            else if (lightMode == LightmapBakeType.Baked) lightTypeString = "Baked";
        }

        // Collider 컴포넌트 
        objects.GameObjects.Add(engineGameObject);
        //SaveJsonData(engineGameObject);

        // 자식 재귀 탐색 
        for (int i = 0; i < go.transform.childCount; i++)
        {
            Transform child = go.transform.GetChild(i);
            GameObject childObject = child.gameObject;
            PrintGameObject(ref objects, childObject);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        Scene scene = SceneManager.GetActiveScene();
        GameObject[] rootObjects = scene.GetRootGameObjects();

        if (!Directory.Exists(Application.dataPath + "/EngineData/"))
        {
            Directory.CreateDirectory(Application.dataPath + "/EngineData/");
        }

        EngineGameObjects objects = new EngineGameObjects();
        objects.GameObjects = new List<EngineGameObject>();

        foreach (GameObject rootObject in rootObjects)
        {
            //Debug.Log(rootObject.name);
            PrintGameObject(ref objects, rootObject);
        }

        SaveJsonData(objects);
    }

    void SaveJsonData(EngineGameObjects data)
    {
        string jsonData = JsonUtility.ToJson(data, true);
        string path = Path.Combine(Application.dataPath + "/EngineData/", "Exportdata" + ".json");
        File.WriteAllText(path, jsonData);
        Debug.Log(path);

        Debug.Log("Data Saved: " + jsonData);
    }

    string ResourcePathSetting(string path)
    {
        string keyword = "resource/";

        int index = path.IndexOf(keyword);
        string result = "";
        if (index != -1)
        {
            result = path.Substring(index);
            //Debug.Log(result);
        }
        else
        {
            //Debug.Log("Keyword not found.");
        }

        return result;
    }
}

