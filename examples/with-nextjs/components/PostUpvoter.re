%raw
"require('../styles/main.css')";

open Util.ReactStuff;

module UpdatePostConfig = [%graphql
  {|
    mutation updatePost($id: ID!, $votes: Int) {
      updatePost(id: $id, votes: $votes) {
        id
        __typename
        votes
      }
    }
  |}
];

module UpdatePostMutation = GraphqlHooksMutation.Make(UpdatePostConfig);

[@react.component]
let make = (~id: string, ~votes: int, ~onUpdate: unit => unit) => {
  let (updatePost, _, _) =
    UpdatePostMutation.use(
      ~variables=
        UpdatePostConfig.make(
          ~id,
          ~votes={
            votes + 1;
          },
          (),
        )##variables,
      (),
    );

  <button
    className="votes--button"
    onClick={_ =>
      updatePost()
      |> Js.Promise.then_(_result => onUpdate() |> Js.Promise.resolve)
      |> ignore
    }>
    {votes->string_of_int->s}
  </button>;
};