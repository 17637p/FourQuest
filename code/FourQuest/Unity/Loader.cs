using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;

struct EngineObject
{
    public string Name;
    public string ParentName;
    public Vector3 Position;
    public Quaternion Rotation;
    public Vector3 Scale;
    public string MeshName;
    public string ModelPath;
    public bool IsLight;
    public LightType LightType;
    public Color LightColor;
    public float Intensity;
    public float Range;
    public float Spot;
}

public class Loader : MonoBehaviour
{
    public GameObject EmptyObject;

    void EdtRead(ref List<EngineObject> objects, string filePath)
    {
        if (File.Exists(filePath))
        {
            StreamReader reader = new StreamReader(filePath);
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                EngineObject obj = new EngineObject();
                
                // object 이름
                obj.Name = line;
                obj.ParentName = "";

                // 부모
                string isParent = reader.ReadLine();
                if(isParent == "true")
                {
                    obj.ParentName = reader.ReadLine();
                }

                // Transform
                string position = reader.ReadLine();
                string[] positionSplit = position.Split('\x020');
                obj.Position.x = float.Parse(positionSplit[0]);
                obj.Position.y = float.Parse(positionSplit[1]);
                obj.Position.z = float.Parse(positionSplit[2]);

                string rotation = reader.ReadLine();
                string[] rotationSplit = rotation.Split('\x020');
                obj.Rotation.x = float.Parse(rotationSplit[0]);
                obj.Rotation.y = float.Parse(rotationSplit[1]);
                obj.Rotation.z = float.Parse(rotationSplit[2]);
                obj.Rotation.w = float.Parse(rotationSplit[3]);

                string scale = reader.ReadLine();
                string[] scaleSplit = scale.Split('\x020');
                obj.Scale.x = float.Parse(scaleSplit[0]);
                obj.Scale.y = float.Parse(scaleSplit[1]);
                obj.Scale.z = float.Parse(scaleSplit[2]);

                // StaticRenderer
                string isMeshName = reader.ReadLine();
                obj.MeshName = "";
                if(isMeshName == "meshNameTrue")
                {
                    obj.MeshName = reader.ReadLine();
                }

                string isMesh = reader.ReadLine();
                obj.ModelPath = "";
                if(isMesh == "Statictrue")
                {
                    obj.ModelPath = reader.ReadLine();
                    if(obj.ModelPath == "NULL")
                    {
                        obj.ModelPath = "";
                    }
                }

                // Light
                string isLight = reader.ReadLine();
                obj.IsLight = false;
                if (isLight == "Lighttrue")
                {
                    obj.IsLight = true;
                    string lightType = reader.ReadLine();
                    if(lightType == "Directional")
                    {
                        obj.LightType = LightType.Directional;
                    }
                    else if(lightType == "Point")
                    {
                        obj.LightType = LightType.Point;
                    }
                    else if (lightType == "Spot")
                    {
                        obj.LightType = LightType.Spot;
                    }

                    string color = reader.ReadLine();
                    string[] colorSplit = color.Split('\x020');
                    obj.LightColor.r = float.Parse(colorSplit[0]);
                    obj.LightColor.g = float.Parse(colorSplit[1]);
                    obj.LightColor.b = float.Parse(colorSplit[2]);
                    obj.LightColor.a = 1;

                    obj.Intensity = float.Parse(reader.ReadLine());
                    obj.Range = float.Parse(reader.ReadLine());
                    obj.Spot = float.Parse(reader.ReadLine());
                }

                reader.ReadLine(); // End

                objects.Add(obj);

                //Debug.Log(line);
            }
        }
    }

    void CreateGameObject(ref List<EngineObject> objects)
    {
        foreach(EngineObject obj in objects)
        {
            //Debug.Log(obj.MeshName);
            if (obj.ModelPath != "")
            {
                string[] modelPath = obj.ModelPath.Split('.');
                //Debug.Log(modelPath[0]);
                Object loadObject = Resources.Load(modelPath[0]);
                if (loadObject != null)
                {
                    GameObject loadGameObject = (GameObject)Instantiate(loadObject);
                    for (int i = 0; i < loadGameObject.transform.childCount; i++)
                    {
                        //staticMeshObject.transform.GetChild(i).name = "autoCreate";
                        loadGameObject.transform.GetChild(i).gameObject.AddComponent<autoCreate>();
                    }
                    GameObject staticMeshObject = loadGameObject;

                    staticMeshObject.name = obj.Name;

                    if (obj.ParentName != "")
                    {
                        GameObject parentObject = GameObject.Find(obj.ParentName);
                        staticMeshObject.transform.parent = parentObject.transform;
                        Debug.Log(obj.Name);
                    }

                    staticMeshObject.transform.localPosition = obj.Position;
                    staticMeshObject.transform.localRotation = obj.Rotation;
                    staticMeshObject.transform.localScale = obj.Scale;

                    if (!Directory.Exists(Application.dataPath + "Assets/Test/"))
                    {
                        Directory.CreateDirectory(Application.dataPath + "Assets/Test/");
                    }
                    //PrefabUtility.SaveAsPrefabAsset(staticMeshObject, "Assets/Test/" + obj.Name + ".prefab");
                    MeshFilter filter = staticMeshObject.GetComponent<MeshFilter>();
                    MeshRenderer meshRenderer = staticMeshObject.GetComponent<MeshRenderer>();

                    if(filter != null)
                    {
                        Destroy(filter);
                        Destroy(meshRenderer);
                    }
                }
                else
                {
                    //Debug.Log(modelPath[0] + "이 없습니다!");
                }
            }
            else if(obj.IsLight)
            {
                GameObject instance = Instantiate(EmptyObject);
                instance.name = obj.Name;

                Light light = instance.AddComponent<Light>();
                light.type = obj.LightType;
                light.intensity = obj.Intensity;
                light.range = obj.Range;
                light.spotAngle = obj.Spot;

                instance.transform.localPosition = obj.Position;
                instance.transform.localRotation = obj.Rotation;
                instance.transform.localScale = obj.Scale;
            }
            else if(obj.MeshName != "" && obj.ParentName != "")
            {
                GameObject parentObject = GameObject.Find(obj.ParentName);
                if(parentObject != null)
                {
                    GameObject sameMeshObject = EmptyObject;

                    Scene scene = SceneManager.GetActiveScene();
                    MeshFilter[] meshFilters = FindObjectsOfType<MeshFilter>();

                    string[] nameSplit = obj.Name.Split("(");
                    Debug.Log("nameSplit[0]" + nameSplit[0]);

                    foreach (MeshFilter meshfilter in meshFilters)
                    {
                        //Debug.Log("obj.MeshName" + obj.MeshName);
                        //Debug.Log("nameSplit[0]" + nameSplit[0]);
                        if (((meshfilter.sharedMesh.name == obj.MeshName) || (meshfilter.sharedMesh.name == nameSplit[0])) && (meshfilter.gameObject.transform.parent == parentObject.transform))
                        {
                            //Debug.Log(meshfilter.sharedMesh.name);
                            //Debug.Log("obj.MeshName" + obj.MeshName);
                            //Debug.Log("nameSplit[0]" + nameSplit[0]);
                            //Debug.Log(words[0]);
                            sameMeshObject = meshfilter.gameObject;
                        }
                    }

                    if (sameMeshObject == EmptyObject)
                    {
                        foreach (MeshFilter meshfilter in meshFilters)
                        {
                            //Debug.Log("obj.MeshName" + obj.MeshName);
                            //Debug.Log("nameSplit[0]" + nameSplit[0]);
                            if (meshfilter.sharedMesh.name == obj.MeshName)
                            {
                                //Debug.Log(meshfilter.sharedMesh.name);
                                //Debug.Log("obj.MeshName" + obj.MeshName);
                                //Debug.Log("nameSplit[0]" + nameSplit[0]);
                                //Debug.Log(words[0]);
                                sameMeshObject = meshfilter.gameObject;
                            }
                        }
                    }

                    if (sameMeshObject == EmptyObject)
                    {
                        foreach (MeshFilter meshfilter in meshFilters)
                        {
                            //Debug.Log("obj.MeshName" + obj.MeshName);
                            //Debug.Log("nameSplit[0]" + nameSplit[0]);
                            if (meshfilter.sharedMesh.name == nameSplit[0])
                            {
                                //Debug.Log(meshfilter.sharedMesh.name);
                                //Debug.Log("obj.MeshName" + obj.MeshName);
                                //Debug.Log("nameSplit[0]" + nameSplit[0]);
                                //Debug.Log(words[0]);
                                sameMeshObject = meshfilter.gameObject;
                            }
                        }
                    }

                    if(sameMeshObject == EmptyObject)
                    {
                        autoCreate[] autoCreates = FindObjectsOfType<autoCreate>();
                        foreach (autoCreate autoCre in autoCreates)
                        {
                            if (autoCre.gameObject.name == nameSplit[0])
                            {
                                if (autoCre.gameObject.GetComponent<MeshFilter>() != null)
                                {
                                    sameMeshObject = autoCre.gameObject;
                                }
                            }
                        }
                    }

                    GameObject newMeshObject = Instantiate(sameMeshObject);

                    autoCreate autoCrere= newMeshObject.GetComponent<autoCreate>();
                    if(autoCrere != null)
                    {
                        Destroy(autoCrere);
                    }

                    foreach (Transform child in newMeshObject.transform)
                    {
                        Destroy(child.gameObject);
                    }

                    newMeshObject.name = obj.Name;
                    //newMeshObject.name = "Child";

                    //Debug.Log(obj.ParentName);
                    newMeshObject.transform.parent = parentObject.transform;

                    newMeshObject.transform.localPosition = obj.Position;
                    //newMeshObject.transform.localPosition = new Vector3(-obj.Position.x, obj.Position.y, -obj.Position.z);
                    //newMeshObject.transform.localRotation = obj.Rotation;

                    // x축과 z축 회전을 뒤집기
                    float invertedX = obj.Rotation.eulerAngles.x;
                    float invertedZ = obj.Rotation.eulerAngles.z;
                    //float invertedZ = obj.Rotation.eulerAngles.z;

                    // 새로운 회전값 설정
                    newMeshObject.transform.localRotation = Quaternion.Euler(invertedX, obj.Rotation.eulerAngles.y, invertedZ);

                    newMeshObject.transform.localScale = obj.Scale;
                }
            }
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        string filePath = Application.dataPath + "/Resources/resource/Export/EngineData.edt";
        List<EngineObject> objects = new List<EngineObject>();

        EdtRead(ref objects, filePath);
        CreateGameObject(ref objects);

        //// 씬 내 모든 게임 오브젝트를 찾음
        //GameObject[] allObjects = FindObjectsOfType<GameObject>();
        //
        //// 이름이 targetName과 일치하는 오브젝트 리스트
        //List<GameObject> matchingObjects = new List<GameObject>();
        //
        //// 모든 오브젝트를 순회하며 이름이 일치하는지 확인
        //foreach (GameObject obj in allObjects)
        //{
        //    if (obj.name == "autoCreate")
        //    {
        //        matchingObjects.Add(obj);
        //    }
        //}
        //for (int i = 0; i < matchingObjects.Count; i++)
        //{
        //    Destroy(matchingObjects[i]);
        //}

        //GameObject[] allObjects = FindObjectsOfType<GameObject>();
        //foreach(GameObject obj in allObjects)
        //{
        //    if(obj.GetComponent<autoCreate>() != null)
        //    {
        //        Destroy(obj);
        //    }
        //}
    }

    void Update()
    {
        if(Input.GetKeyDown(KeyCode.P))
        {
            GameObject[] allObjects = FindObjectsOfType<GameObject>();
            foreach (GameObject obj in allObjects)
            {
                if (obj.GetComponent<autoCreate>() != null)
                {
                    Destroy(obj);
                }
            }
        }

        if (Input.GetKeyDown(KeyCode.O))
        {
            Scene scene = SceneManager.GetActiveScene();
            GameObject[] rootObjects = scene.GetRootGameObjects();

            foreach (GameObject obj in rootObjects)
            {
                PrefabUtility.SaveAsPrefabAsset(obj, "Assets/Test/" + obj.name + ".prefab");
            }
        }
    }
}
